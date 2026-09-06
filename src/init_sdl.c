#include "init_sdl.h"
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_render.h>

bool game_init_sdl(struct Game *g) {
    SDL_AudioSpec spec;

    if (!SDL_Init(SDL_FLAGS)) {
        fprintf(stderr, "Error initializing SDL3: %s\n", SDL_GetError());
        return false;
    }

    g->window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!g->window) {
        fprintf(stderr, "Error while creating window: %s\n", SDL_GetError());
        return false;
    }

    g->renderer = SDL_CreateRenderer(g->window, NULL);
    if (!g->renderer) {
        fprintf(stderr, "Error while creating renderer: %s\n", SDL_GetError());
        return false;
    }


    spec.channels = 1;
    spec.format = SDL_AUDIO_F32;
    spec.freq = 8000;
 

    g->audio_stream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (!g->audio_stream) {
        fprintf(stderr, "Error while creating audio stream: %s\n",
                SDL_GetError());
        return false;
    }

    SDL_ResumeAudioStreamDevice(g->audio_stream);
    return true;
}
