#include "shot.h"

void free_shot(Shot *shot)
{
    free(shot->data_);
    shot->data_ = NULL;
}

void shot_process(Shot *shot, struct Environment *env)
{
    switch (shot->type)
    {
    case SHOT_SHELL:
    {
        struct ShellShotData *data = shot->data_;
        data->life += env->dt;
        if (data->life > data->maxlife)
        {
            shot->destroyed = 1;
        }
        else
        {
            shot->x += data->vx * env->dt;
            shot->y += data->vy * env->dt;
        }
        break;
    }
    }
}

int shot_droid_hit(const Shot *shot, const DroidState *droid)
{
    switch (shot->type)
    {
    case SHOT_SHELL:
    {
        float rx = shot->x - droid->x;
        float ry = shot->y - droid->y;
        return vec_length(rx, ry) <= ((struct ShellShotData *)shot->data_)->radius;
    }
    }
    return 0;
}

Shot *make_shell_shot(int shot_by, int attack, float x, float y, float vx, float vy, float radius)
{
    Shot *shot = (Shot *)malloc(sizeof(Shot));
    shot->attack = attack;
    shot->shot_by = shot_by;
    shot->destroyed = 0;
    shot->type = SHOT_SHELL;
    shot->x = x;
    shot->y = y;
    shot->data_ = malloc(sizeof(struct ShellShotData));
    struct ShellShotData *data = (struct ShellShotData *)shot->data_;
    data->life = 0.0;
    data->maxlife = 6.0;
    data->vx = vx;
    data->vy = vy;
    data->radius = radius;
    return shot;
}
