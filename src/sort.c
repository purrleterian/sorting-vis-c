#include "sort.h"
#include "bars.h"
#include "game.h"
#include <SDL3/SDL_atomic.h>
#include <SDL3/SDL_mutex.h>

static int current_sine_sample = 0;
long comp = 0;
long atr = 0;

char *sort_name = "(none)";

static int freq_for_value(float bar_n) {
    // bar_n is 0..1 -> map to roughly 150Hz..1000Hz, weighted toward
    // perceptually even steps rather than linear Hz
    float min_f = 150.0f, max_f = 1000.0f;
    return (int)(min_f * SDL_powf(max_f / min_f, bar_n));
}

static void play_beep(int freq) {

    // example from SDL3 website, this just feels the sample buffer with a sine
    // wave.
    static float samples[512]; /* this will feed 512 samples each frame until we
                                  get to our maximum. */

    // Don't pile more audio on top of a backlog — keep audio in sync with
    // visuals
    if (SDL_GetAudioStreamQueued(stream) > (int)sizeof(samples) * 2) {
        return;
    }

    /* generate a 440Hz pure tone */
    for (int i = 0; i < SDL_arraysize(samples); i++) {
        const float phase = current_sine_sample * (freq) / 8000.0f;
        samples[i] = SDL_sinf(phase * 2 * SDL_PI_F);
        current_sine_sample++;
    }

    /* wrapping around to avoid floating-point errors */
    current_sine_sample %= 8000;

    SDL_PutAudioStreamData(stream, samples, sizeof(samples));
}

static void step(Bars *b, int _i, int _j) {
    SDL_LockMutex(b->lock);
    b->hi1 = _i;
    b->hi2 = _j;

    SDL_UnlockMutex(b->lock);

    play_beep(freq_for_value(b->bar_n[_i]));
    play_beep(freq_for_value(b->bar_n[_j]));

    // while is paused, wait
    while (SDL_GetAtomicInt(&b->paused) && !SDL_GetAtomicInt(&b->quit)) {
        SDL_Delay(10);
    }

    if (!SDL_GetAtomicInt(&b->quit)) {
        SDL_DelayNS((Uint64)(b->delay_ms * 1000000.0f));
    }
}

static void after_sort(Bars *b) {
    float prev = b->delay_ms;
    b->delay_ms = 15;
    for (int i = 0; i < b->total; i++) {
        step(b, i, i);
    }

    b->delay_ms = prev;
}

int selection_sort_thread(void *data) {
    sort_name = "Selection Sort";
    Bars *b = (Bars *)data;
    int n = b->total;

    atr = 0;
    comp = 0;

    for (int i = 0; i < n - 1 && !SDL_GetAtomicInt(&b->quit); i++) {
        int min_i = i;
        for (int j = i + 1; j < n && !SDL_GetAtomicInt(&b->quit); j++) {
            step(b, j, min_i);
            comp++;
            if (b->bar_n[j] < b->bar_n[min_i]) {
                min_i = j;
            }
        }

        if (!SDL_GetAtomicInt(&b->quit) && min_i != i) {
            SDL_LockMutex(b->lock);

            atr += 3;
            float temp = b->bar_n[min_i];

            b->bar_n[min_i] = b->bar_n[i];
            b->bar_n[i] = temp;

            set_bar_height(b);
            SDL_UnlockMutex(b->lock);
            step(b, i, min_i);
        }
    }

    after_sort(b);

    b->hi1 = b->hi2 = -1;
    SDL_SetAtomicInt(&b->running, 0);
    return 0;
}

int bubble_sort_thread(void *data) {
    sort_name = "Bubble Sort";
    Bars *b = (Bars *)data;
    int n = b->total;

    comp = 0;
    atr = 0;

    for (int i = 0; i < n - 1 && !SDL_GetAtomicInt(&b->quit); i++) {
        for (int j = 0; j < n - i - 1 && !SDL_GetAtomicInt(&b->quit); j++) {
            step(b, j, j + 1);

            SDL_LockMutex(b->lock);
            bool needs_swap = b->bar_n[j] > b->bar_n[j + 1];
            comp++;
            if (needs_swap) {
                atr += 3;
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

    after_sort(b);

    b->hi1 = b->hi2 = -1;
    SDL_SetAtomicInt(&b->running, 0);
    return 0;
}

int insertion_sort_thread(void *data) {
    Bars *b = (Bars *)data;

    int n = b->total;

    sort_name = "Insertion Sort";
    comp = 0;
    atr = 0;

    for (int i = 1; i < n && !SDL_GetAtomicInt(&b->quit); ++i) {
        SDL_LockMutex(b->lock);
        float key = b->bar_n[i];
        SDL_UnlockMutex(b->lock);
        // start one position before the key
        int j = i - 1;

        while (j >= 0 && b->bar_n[j] > key && !SDL_GetAtomicInt(&b->quit)) {
            comp++;
            atr += 1;

            step(b, j, j + 1);

            SDL_LockMutex(b->lock);
            b->bar_n[j + 1] = b->bar_n[j];
            set_bar_height(b);
            SDL_UnlockMutex(b->lock);

            j--;
        }

        SDL_LockMutex(b->lock);
        b->bar_n[j + 1] = key;
        set_bar_height(b);

        step(b, j + 1, i);
        SDL_UnlockMutex(b->lock);
    }

    after_sort(b);
    b->hi1 = b->hi2 = -1;
    SDL_SetAtomicInt(&b->running, 0);
    return 0;
}

static void swap(float *x, float *y) {
    float temp = *x;
    *x = *y;
    *y = temp;
}

static int partition(Bars *b, int low, int high) {
    SDL_LockMutex(b->lock);
    float pivot_value = b->bar_n[high];
    SDL_UnlockMutex(b->lock);
    int i = low;
    for (int j = low; j < high && !SDL_GetAtomicInt(&b->quit); j++) {

        step(b, i, j);
        if (b->bar_n[j] <= pivot_value) {
            SDL_LockMutex(b->lock);
            swap(&b->bar_n[j], &b->bar_n[i]);
            SDL_UnlockMutex(b->lock);
            i++;
        }
    }

    SDL_LockMutex(b->lock);
    swap(&b->bar_n[i], &b->bar_n[high]);
    set_bar_height(b);
    SDL_UnlockMutex(b->lock);
    step(b, i, high);

    return i;
}

static void quick_sort_recurs(Bars *b, int low, int high) {
    if (low < high) {
        int pivot = partition(b, low, high);
        quick_sort_recurs(b, low, pivot - 1);
        quick_sort_recurs(b, pivot + 1, high);
    }
}

int quick_sort_thread(void *data) {
    Bars *b = (Bars *)data;
    int n = b->total;

    quick_sort_recurs(b, 0, n - 1);
    set_bar_height(b);

    after_sort(b);
    b->hi1 = b->hi2 = -1;
    SDL_SetAtomicInt(&b->running, 0);
    return 0;
}
