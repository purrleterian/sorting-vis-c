#include "bars.h"
#include "main.h"
// #include "sort.h"
#include <SDL3/SDL_render.h>

void set_bar_height(Bars *b) {
    float height;
    for (int i = 0; i < b->total; i++) {
        height = map(b->bar_n[i], 0, 1, 0, WINDOW_HEIGHT);

        b->rects[i].w = b->bar_w;
        b->rects[i].h = height;

        b->rects[i].y = WINDOW_HEIGHT - b->rects[i].h;
        b->rects[i].x = (b->bar_w + BAR_GAP) * i;
    }
}

void randomize_bars(Bars *b) {
    SDL_LockMutex(b->lock);

    float random_n;
    float height;

    b->hi1 = b->hi2 = -1;
    float precision = 10000;
    for (int i = 0; i < b->total; i++) {
        random_n = ((rand() % (int) precision + 1) / precision);
        height = map(random_n, 0, 1, 0, WINDOW_HEIGHT);

        b->bar_n[i] = random_n;
        b->rects[i].h = height;

        b->rects[i].y = WINDOW_HEIGHT - b->rects[i].h;

    }

    SDL_UnlockMutex(b->lock);
}

bool bars_new(Bars **bars, SDL_Renderer *renderer) {

    *bars = calloc(1, sizeof(Bars));
    if (bars == NULL) {
        fprintf(stderr, "Error allocating memory: %s\n", SDL_GetError());
        return false;
    }

    Bars *b = *bars;
    b->renderer = renderer;
    b->total = TOTAL_BARS;
    b->bar_w = ((float)WINDOW_WIDTH / b->total) - BAR_GAP;

    b->rects =
        calloc(b->total, sizeof(SDL_FRect));    // store rect info for each bar
    b->bar_n = calloc(b->total, sizeof(float)); // store the n value of each bar
    b->n_pos = calloc(b->total, sizeof(int));

    b->lock = SDL_CreateMutex();
    b->hi1 = b->hi2 = -1;
    b->delay_ms = 5.0f;



    float height, n;
    for (int i = 0; i < b->total; i++) {
        n = 0.5;
        height = map(n, 0, 1, 0, WINDOW_HEIGHT);

        b->bar_n[i] = n;

        b->rects[i].w = b->bar_w;
        b->rects[i].h = height;

        b->rects[i].y = WINDOW_HEIGHT - b->rects[i].h;
        b->rects[i].x = (b->bar_w + BAR_GAP) * i;

        b->n_pos[i] = i;
    }

    printf("Bars init\n");
    return true;
}

void bars_free(Bars **bars) {
    if (*bars) {
        Bars *b = *bars;
        if (b->rects) {
            free(b->rects);
            b->rects = NULL;
        }

        if (b->bar_n) {
            free(b->bar_n);
            b->rects = NULL;
        }

        if (b->n_pos) {
            free(b->n_pos);
            b->n_pos = NULL;
        }

        if (b->lock) {
            SDL_DestroyMutex(b->lock);
            b->lock = NULL;
        }

        b->renderer = NULL;
        free(b);
        *bars = NULL;
        printf("Bars - Free\n");
    }
}

void bars_update(Bars *b) {
    for (int i = 0; i < b->total; i++) {
        // update
    }
}

void bars_draw(Bars *b) {

    SDL_LockMutex(b->lock);

    for (int i = 0; i < b->total; i++) {

        if (b->hi1 == i || b->hi2 == i) {
            SDL_SetRenderDrawColor(b->renderer, 0, 255, 0, 255);
        } else {

            SDL_SetRenderDrawColor(b->renderer, 255, 255, 255, 255);
        }
        SDL_RenderFillRect(b->renderer, &b->rects[i]);
    }

    SDL_UnlockMutex(b->lock);
}
