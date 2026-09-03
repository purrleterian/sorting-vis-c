#include "game.h"
#include "init_sdl.h"
#include <SDL3/SDL_render.h>



void game_events(struct Game *g);
void game_draw(struct Game *g);
void game_update(struct Game *g);


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

    if (!bars_new(&g->bars, g->renderer)) return false;

    g->is_running = true;

    return true;
}

void game_free(struct Game **game) {
    if (*game) {
        struct Game *g = *game;
        if (g->renderer) {
            SDL_DestroyRenderer(g->renderer);
            g->renderer = NULL;
        }

        if (g->window) {
            SDL_DestroyWindow(g->window);
            g->window = NULL;
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
                randomize_bars(g->bars);
                break;

            case SDL_SCANCODE_S:
                selection_sort(g->bars, sort_step);
                sort_step++;

            default:
                break;
            }
            break;
        default:
            break;
        }
    }
}

void game_update(struct Game *g) {
    bars_update(g->bars);
}


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

        SDL_Delay(16);
    }
}

