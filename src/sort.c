#include "sort.h"
#include "bars.h"
#include <SDL3/SDL_atomic.h>
#include <SDL3/SDL_mutex.h>

static void step(Bars *b, int i, int j) {
    SDL_LockMutex(b->lock);
    b->hi1 = i;
    b->hi2 = j;
    SDL_UnlockMutex(b->lock);

    while (SDL_GetAtomicInt(&b->paused) && !SDL_GetAtomicInt(&b->quit)) {
        SDL_Delay(10);
    }

    if (!SDL_GetAtomicInt(&b->quit)) {
        SDL_DelayNS((Uint64)(b->delay_ms * 1000000.0f));
    }
}

int selection_sort_thread(void *data) {
    Bars *b = (Bars *)data;
    int n = b->total;

    for (int i = 0; i < n - 1 && !SDL_GetAtomicInt(&b->quit); i++) {
        int min_i = i;
        for (int j = i + 1; j < n && !SDL_GetAtomicInt(&b->quit); j++) {
            step(b, j, min_i);
            if (b->bar_n[j] < b->bar_n[min_i]) {
                min_i = j;
            }
        }

        if (!SDL_GetAtomicInt(&b->quit) && min_i != i) {
            SDL_LockMutex(b->lock);
            float temp = b->bar_n[min_i];

            b->bar_n[min_i] = b->bar_n[i];
            b->bar_n[i] = temp;

            set_bar_height(b);
            SDL_UnlockMutex(b->lock);
            step(b, i, min_i);
        }
    }
    b->hi1 = b->hi2 = -1;
    SDL_SetAtomicInt(&b->running, 0);
    return 0;
}

int bubble_sort_thread(void *data) {
    Bars *b = (Bars *)data;
    int n = b->total;

    for (int i = 0; i < n - 1 && !SDL_GetAtomicInt(&b->quit); i++) {
        for (int j = 0; j < n - 1 && !SDL_GetAtomicInt(&b->quit); j++) {
            step(b, j, j + 1);

            SDL_LockMutex(b->lock);
            bool needs_swap = b->bar_n[j] > b->bar_n[j + 1];
            if (needs_swap) {
                float tmp = b->bar_n[j];
                b->bar_n[j] = b->bar_n[j + 1];
                b->bar_n[j + 1] = tmp;
                set_bar_height(b);
            }
            SDL_UnlockMutex(b->lock);

            if (needs_swap)
                step(b, j, j + 1); // show the swap
        }
    }

    b->hi1 = b->hi2 = -1;
    SDL_SetAtomicInt(&b->running, 0);
    return 0;
}
