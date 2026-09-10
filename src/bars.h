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

    float gap;
} Bars;

bool bars_new(Bars **bars, SDL_Renderer *renderer);
void bars_free(Bars **bars);
void bars_draw(Bars *b);
void bars_update(Bars *b);

void randomize_bars(Bars *b);
void shuffle_bars(Bars *b);
void set_bar_height(Bars *b);
void update_bars_pos(Bars *b);
void change_bar_n(Bars *b, int n);


extern int total_bars;

#endif
