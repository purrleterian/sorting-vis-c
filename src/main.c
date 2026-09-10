#include "game.h"


int main(int argc, char **argv) {
    if (argc > 1) {
        total_bars = atoi(argv[1]);
    }

    srand(time(NULL));
    int exit_status = EXIT_FAILURE;
    srand(time(NULL));

    struct Game *game = NULL;

    if (game_new(&game)) {
        game_run(game);
        exit_status = EXIT_SUCCESS;
    }


    game_free(&game);
    return exit_status;
}
