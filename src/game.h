#ifndef GAME_H
#define GAME_H

#include "main.h"
#include "bars.h"
#include "sort.h"

struct Game {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;
    SDL_AudioStream *audio_stream;

    Bars *bars;
    bool is_running;

    SDL_Thread *sort_thread;
};

void game_free(struct Game **game);
bool game_new(struct Game **game);
void game_run(struct Game *g);

#endif
