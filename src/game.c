#include "game.h"
#include "init_sdl.h"
#include "sort.h"
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_render.h>

void game_events(struct Game *g);
void game_draw(struct Game *g);
void game_update(struct Game *g);

SDL_AudioStream *stream = NULL;

static void start_sort(struct Game *g, SDL_ThreadFunction fn) {

    if (SDL_GetAtomicInt(&g->bars->running))
        return; // one sort at a time

    SDL_SetAtomicInt(&g->bars->quit, 0);
    SDL_SetAtomicInt(&g->bars->paused, 0);
    SDL_SetAtomicInt(&g->bars->running, 1);
    g->sort_thread = SDL_CreateThread(fn, "SortThread", g->bars);
}

static void stop_sort(struct Game *g) {
    if (g->sort_thread) {
        SDL_SetAtomicInt(&g->bars->quit, 1);
        SDL_SetAtomicInt(&g->bars->paused,
                         0); // wake it if paused, so it can see quit
        SDL_WaitThread(g->sort_thread, NULL);
        g->sort_thread = NULL;
        SDL_SetAtomicInt(&g->bars->running, 0);
    }
}

bool game_new(struct Game **game) {
    *game = calloc(1, sizeof(struct Game));
    if (*game == NULL) {
        fprintf(stderr, "Error while allocating memory (new game)\n");
        return false;
    }
    struct Game *g = *game;
    if (!game_init_sdl(g)) {
        return false;
    }

    if (!bars_new(&g->bars, g->renderer))
        return false;

    g->is_running = true;

    return true;
}

void game_free(struct Game **game) {
    if (*game) {
        struct Game *g = *game;
        stop_sort(g);
        if (g->renderer) {
            SDL_DestroyRenderer(g->renderer);
            g->renderer = NULL;
        }

        if (g->window) {
            SDL_DestroyWindow(g->window);
            g->window = NULL;
        }

        if (stream) {
            SDL_DestroyAudioStream(stream);
            stream = NULL;
        }

        if (g->bars) {
            bars_free(&g->bars);
        }

        SDL_Quit();
        free(g);

        g = NULL;
        *game = NULL;
        printf("Game memory cleared\n");
    }
}

void game_events(struct Game *g) {

    while (SDL_PollEvent(&g->event)) {
        switch (g->event.type) {
        case SDL_EVENT_QUIT:
            g->is_running = false;
            break;

        case SDL_EVENT_KEY_DOWN:
            switch (g->event.key.scancode) {
            case SDL_SCANCODE_ESCAPE:
                g->is_running = false;
                break;

            case SDL_SCANCODE_R:
                stop_sort(g);
                randomize_bars(g->bars);
                break;

            case SDL_SCANCODE_S:
                start_sort(g, selection_sort_thread);
                break;

            case SDL_SCANCODE_D:
                start_sort(g, bubble_sort_thread);
                break;

            case SDL_SCANCODE_SPACE: {
                int cur = SDL_GetAtomicInt(&g->bars->paused);
                SDL_SetAtomicInt(&g->bars->paused, !cur);
                break;
            }

            case SDL_SCANCODE_UP:
                g->bars->delay_ms = SDL_max(0, g->bars->delay_ms - 0.1f);

                printf("%.3f\n", g->bars->delay_ms);
                break;
            case SDL_SCANCODE_DOWN:
                g->bars->delay_ms += 0.1;

                printf("%.3f\n", g->bars->delay_ms);
                break;

            default:
                break;
            }
            break;
        default:
            break;
        }
    }
}

void game_update(struct Game *g) { bars_update(g->bars); }

void game_draw(struct Game *g) {
    SDL_RenderClear(g->renderer);

    // draw here
    bars_draw(g->bars);
    SDL_RenderPresent(g->renderer);

    SDL_SetRenderDrawColor(g->renderer, 34, 35, 35, 255);
    SDL_SetRenderViewport(g->renderer, NULL);
}

void game_run(struct Game *g) {
    while (g->is_running) {
        game_events(g);

        game_update(g);
        game_draw(g);

        SDL_Delay(0);
    }
}
