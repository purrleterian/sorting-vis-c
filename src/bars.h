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
} Bars;

bool bars_new(Bars **bars, SDL_Renderer *renderer);
void bars_free(Bars **bars);
void bars_draw(Bars *b);
void bars_update(Bars *b);

void randomize_bars(Bars *b);

#endif
