#include <SDL.h>
#include "droid.h"
#include "shot.h"


typedef struct {
    DroidState **droids;
    size_t n;
    size_t cap;
} DroidList;

DroidList *init_droid_list();
void free_droid_list(DroidList *list);
void droid_list_add(DroidList *list, DroidState *droid);
void droid_list_remove_destroyed(DroidList *list);

typedef struct {
    Shot **shots;
    size_t n;
    size_t cap;
} ShotList;

ShotList *init_shot_list();
void free_shot_list(ShotList *list);
void shot_list_add(ShotList *list, Shot *shot);
void shot_list_remove_destroyed(ShotList *list);

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    DroidList *droids;
    ShotList *shots;
    uint32_t lasttime;
} GameInstance;

GameInstance *new_game_instance();
void free_game_instance(GameInstance *game);

void game_add_droid(GameInstance *game, DroidState *droid);
void game_add_shot(GameInstance *game, Shot *shot);

void game_run(GameInstance *game);
