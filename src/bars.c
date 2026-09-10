#include "bars.h"
#include "main.h"
// #include "sort.h"
#include <SDL3/SDL_render.h>

int total_bars = 50;

void set_bar_height(Bars *b) {
    float height;
    for (int i = 0; i < b->total; i++) {
        height = map(b->bar_n[i], 0, 1, 0, WINDOW_HEIGHT - MARGIN_TOP);

        b->rects[i].w = b->bar_w;
        b->rects[i].h = height;

        b->rects[i].y = WINDOW_HEIGHT - b->rects[i].h;
        b->rects[i].x = (b->bar_w + b->gap) * i;
    }
}

void randomize_bars(Bars *b) {
    SDL_LockMutex(b->lock);

    float random_n;
    float height;

    b->hi1 = b->hi2 = -1;
    float precision = 10000;
    for (int i = 0; i < b->total; i++) {
        random_n = ((rand() % (int)precision + 1) / precision);
        height = map(random_n, 0, 1, 0, WINDOW_HEIGHT - MARGIN_TOP);

        b->bar_n[i] = random_n;
        b->rects[i].h = height;

        b->rects[i].y = WINDOW_HEIGHT - b->rects[i].h;
    }

    SDL_UnlockMutex(b->lock);
}

void shuffle_bars(Bars *b) {
    SDL_LockMutex(b->lock);

    b->hi1 = b->hi2 = -1;

    // assign ints;
    for (int i = 0; i < b->total; i++) {
        b->bar_n[i] = map(i, 0, b->total, 0, 1);
    }

    // shuffle;
    for (int i = b->total - 1; i > 0; i--) {
        // pick a random index;
        int rand_i = (rand() % i);

        float temp = b->bar_n[i];
        b->bar_n[i] = b->bar_n[rand_i];
        b->bar_n[rand_i] = temp;
    }
    set_bar_height(b);

    SDL_UnlockMutex(b->lock);
}

void update_bars_pos(Bars *bars) {
    for (int i = 0; i < bars->total; i++) {
        bars->bar_w = ((float)WINDOW_WIDTH / bars->total) - bars->gap;
        bars->rects[i].w = bars->bar_w;

        bars->rects[i].w = bars->bar_w;

        bars->rects[i].y = WINDOW_HEIGHT - bars->rects[i].h;
        bars->rects[i].x = (bars->bar_w + bars->gap) * i;


    }
}

void change_bar_n(Bars *bars, int n) {
    if ((bars->total + n) > bars->total) {
        // new size = old size + however many were added
        bars->rects = realloc(bars->rects, (bars->total * sizeof(SDL_FRect)) + (sizeof(SDL_FRect) * n));
        bars->bar_n = realloc(bars->bar_n, (bars->total * sizeof(float)) + (sizeof(float) * n));
        bars->n_pos = realloc(bars->n_pos, (bars->total * sizeof(int)) + (sizeof(int) * n));
    }

    bars->total = SDL_max(0, bars->total + n);
    update_bars_pos(bars);
    set_bar_height(bars);

    // realocate memory if the amount changed
}

bool bars_new(Bars **bars, SDL_Renderer *renderer) {

    *bars = calloc(1, sizeof(Bars));
    if (bars == NULL) {
        fprintf(stderr, "Error allocating memory: %s\n", SDL_GetError());
        return false;
    }

    Bars *b = *bars;
    b->renderer = renderer;
    b->total = total_bars;

    b->gap = 1;
    b->bar_w = ((float)WINDOW_WIDTH / b->total) - b->gap;

    b->rects =
        calloc(b->total, sizeof(SDL_FRect));    // store rect info for each bar
    b->bar_n = calloc(b->total, sizeof(float)); // store the n value of each bar
    b->n_pos = calloc(b->total, sizeof(int));

    b->lock = SDL_CreateMutex();
    b->hi1 = b->hi2 = -1;
    b->delay_ms = 1.0f;


    float height, n;
    for (int i = 0; i < b->total; i++) {
        n = 0.5;
        height = map(n, 0, 1, 0, WINDOW_HEIGHT - MARGIN_TOP);

        b->bar_n[i] = n;

        b->rects[i].w = b->bar_w;
        b->rects[i].h = height;

        b->rects[i].y = WINDOW_HEIGHT - b->rects[i].h;
        b->rects[i].x = (b->bar_w + b->gap) * i;

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
