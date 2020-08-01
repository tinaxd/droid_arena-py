#include <SDL.h>
#include "droid.h"



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
    SDL_Window *window;
    SDL_Renderer *renderer;
    DroidList *droids;
    uint32_t lasttime;
} GameInstance;

GameInstance *new_game_instance();
void free_game_instance(GameInstance *game);

void game_add_droid(GameInstance *game, DroidState *droid);

void game_run(GameInstance *game);
