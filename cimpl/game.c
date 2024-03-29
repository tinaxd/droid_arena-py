#include "game.h"
#include "env.h"
#include "droid.h"
#include <SDL2_gfxPrimitives.h>

DroidList *init_droid_list()
{
    DroidList *list = (DroidList *)malloc(sizeof(DroidList));
    list->n = 0;
    list->cap = 2;
    list->droids = (DroidState **)malloc(sizeof(DroidState *) * list->cap);
    return list;
}

void free_droid_list(DroidList *list)
{
    for (size_t i = 0; i < list->n; i++)
    {
        free_droid(list->droids[i]);
        free(list->droids[i]);
    }
    free(list->droids);
    list->n = 0;
    list->cap = 0;
    list->droids = NULL;
}

void droid_list_add(DroidList *list, DroidState *droid)
{
    if (list->n >= list->cap)
    {
        list->cap *= 2;
        list->droids = (DroidState **)realloc(list->droids, sizeof(DroidState *) * list->cap);
    }

    list->droids[list->n] = droid;
    list->n++;
}

//void droid_list_remove_destroyed(DroidList *list);

ShotList *init_shot_list()
{
    ShotList *list = (ShotList *)malloc(sizeof(ShotList));
    list->n = 0;
    list->cap = 2;
    list->shots = (Shot **)malloc(sizeof(Shot *) * list->cap);
    return list;
}

void free_shot_list(ShotList *list)
{
    for (size_t i = 0; i < list->n; i++)
    {
        free_shot(list->shots[i]);
        free(list->shots[i]);
    }
    free(list->shots);
    list->n = 0;
    list->cap = 0;
    list->shots = NULL;
}

void shot_list_add(ShotList *list, Shot *shot)
{
    if (list->n >= list->cap)
    {
        list->cap *= 2;
        list->shots = (Shot **)realloc(list->shots, sizeof(Shot *) * list->cap);
    }

    list->shots[list->n] = shot;
    list->n++;
}

void shot_list_remove_destroyed(ShotList *list)
{
    int removed = 0;
    size_t index = 0;
    for (size_t i=0; i<list->n; i++) {
        if (list->shots[i]->destroyed) {
            index = i;
            removed = 1;
            break;
        }
    }

    if (removed) {
        Shot *tmp = list->shots[index];
        list->shots[index] = list->shots[list->n - 1];
        list->shots[list->n - 1] = tmp;
        free_shot(list->shots[list->n - 1]);
        list->shots[list->n - 1] = NULL;
        list->n--;
        shot_list_remove_destroyed(list);
    }
}

GameInstance *new_game_instance()
{
    GameInstance *game = (GameInstance *)malloc(sizeof(GameInstance));
    assert(game != NULL);

    game->window = SDL_CreateWindow("Droid Arena",
                                    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                    640, 640, SDL_WINDOW_SHOWN);
    if (!game->window)
    {
        return NULL;
    }

    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!game->renderer)
    {
        SDL_DestroyWindow(game->window);
        return NULL;
    }

    game->droids = init_droid_list();
    assert(game->droids != NULL);

    game->shots = init_shot_list();
    assert(game->shots != NULL);

    game->lasttime = SDL_GetTicks();
    return game;
}

void free_game_instance(GameInstance *game)
{
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    free_droid_list(game->droids);
    free(game->droids);
}

void game_add_droid(GameInstance *game, DroidState *droid)
{
    droid_list_add(game->droids, droid);
}

void game_add_shot(GameInstance *game, Shot *shot)
{
    shot_list_add(game->shots, shot);
}

void game_process(GameInstance *game, uint32_t delta);
void game_process_collision(GameInstance *game, uint32_t delta);
void game_render(GameInstance *game);

void game_run(GameInstance *game)
{
    int quit = 0;
    while (!quit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = 1;
            }
        }

        uint32_t curr = SDL_GetTicks();
        uint32_t delta = curr - game->lasttime;
        game->lasttime = curr;
        game_process(game, delta);
        game_process_collision(game, delta);
        game_render(game);
        SDL_RenderPresent(game->renderer);
        SDL_Delay(1000 / 60);
    }
}

void game_apply_env(GameInstance *game, struct Environment *env)
{
    for (size_t i=0; i<env->n_shots_queue_; i++) {
        game_add_shot(game, env->shots_queue_[i]);
    }
    
    free(env->shots_queue_);
    env->shots_queue_ = NULL;
    env->n_shots_queue_ = 0;
    env->c_shots_queue_ = 0;
}

void game_process(GameInstance *game, uint32_t delta)
{
    struct Environment env;
    env.dt = delta / 1000.0;
    env.n_droids_ = game->droids->n;
    env.droids_ = game->droids->droids;
    env.n_shots_queue_ = 0;
    env.c_shots_queue_ = 0;
    env.shots_queue_ = NULL;

    for (size_t i = 0; i < game->droids->n; i++)
    {
        next_command(game->droids->droids[i], &env);
    }
    game_apply_env(game, &env);

    for (size_t i = 0; i < game->shots->n; i++)
    {
        shot_process(game->shots->shots[i], &env);
    }
    game_apply_env(game, &env);
}

void game_process_collision(GameInstance *game, uint32_t delta)
{
    for (size_t i = 0; i < game->shots->n; i++)
    {
        Shot *shot = game->shots->shots[i];
        if (shot->destroyed)
            continue;
        for (size_t j = 0; j < game->droids->n; j++)
        {
            DroidState *droid = game->droids->droids[j];
            if (droid->team == shot->shot_by)
                continue;
            if (shot_droid_hit(shot, droid))
            {
                droid_hit(droid, shot);
                shot->destroyed = 1;
                break;
            }
        }
    }
    shot_list_remove_destroyed(game->shots);
}

void game_render(GameInstance *game)
{
    SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
    SDL_RenderClear(game->renderer);

    // Draw droids
    int droid_radius = 30;
    int droid_dot_rad = 5;
    int droid_dot_from_center = 15;
    for (size_t i = 0; i < game->droids->n; i++)
    {
        const DroidState *droid = game->droids->droids[i];
        ColorRGBA color = droid->color;
        filledCircleRGBA(game->renderer, droid->x, droid->y,
                         droid_radius, color.r, color.g, color.b, color.a);
        ColorRGBA cc = color_complement(color);
        filledCircleRGBA(game->renderer,
                         droid->x + droid_dot_from_center * cos(droid->rot),
                         droid->y + droid_dot_from_center * sin(droid->rot),
                         droid_dot_rad, cc.r, cc.g, cc.b, cc.a);
    }

    // Draw shots
    for (size_t i = 0; i < game->shots->n; i++)
    {
        const Shot *shot = game->shots->shots[i];
        switch (shot->type)
        {
        case SHOT_SHELL:
        {
            int radius = ((struct ShellShotData *)shot->data_)->radius;
            filledCircleRGBA(game->renderer,
                             shot->x, shot->y, radius, 0, 0, 0, 255);
        }
        }
    }
}
