#ifndef BARS_H
#define BARS_H

#include "main.h"

typedef struct {
    SDL_Renderer *renderer;
    SDL_FRect *rects;

    float bar_w;
    float *bar_n; // number assosicated
    int *n_pos;

    int total;

    int hi1, hi2;
    SDL_Mutex *lock;
    SDL_AtomicInt paused;
    SDL_AtomicInt quit;
    SDL_AtomicInt running;

    float delay_ms;
} Bars;

bool bars_new(Bars **bars, SDL_Renderer *renderer);
void bars_free(Bars **bars);
void bars_draw(Bars *b);
void bars_update(Bars *b);

void randomize_bars(Bars *b);
void set_bar_height(Bars *b);

#define BAR_GAP 2
#define TOTAL_BARS 100

#endif
