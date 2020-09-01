#include "game.h"

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    GameInstance *game = new_game_instance();

    ColorRGBA d1color = {255, 0, 0, 255};
    DroidCommand d1cmds[] = {
        DROID_CMD_MOVE_F,
        DROID_CMD_TURN_ENEMY,
        DROID_CMD_SHOT_CHARGED_LINEAR,
    };
    DroidState *d1 = make_droid("player", 0, 320, 480, 0, 50, d1color, DROID_COMMAND);
    d1->spec.attack = 5;
    d1->spec.defence = 5;
    d1->spec.movement = 5;
    d1->driver = make_cmd_driver(3, d1cmds);

    ColorRGBA d2color = {0, 255, 0, 255};
    DroidCommand d2cmds[] = {DROID_CMD_TURN_ENEMY, DROID_CMD_SHOT_RANGE, DROID_CMD_TURN_L, DROID_CMD_MOVE_F};
    DroidState *d2 = make_droid("enemy", 1, 320, 160, 0, 50, d2color, DROID_COMMAND);
    d2->spec.attack = 1;
    d2->spec.defence = 5;
    d2->spec.movement = 9;
    d2->driver = make_cmd_driver(4, d2cmds);

    game_add_droid(game, d1);
    game_add_droid(game, d2);
    game_run(game);

    return 0;
}
