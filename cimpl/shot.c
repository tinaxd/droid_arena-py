#include "shot.h"
#include <math.h>

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
    case SHOT_RANGE:
    {
        struct RangeShotData *data = (struct RangeShotData *)shot->data_;
        data->life += env->dt;
        if (data->life > data->max_life) shot->destroyed = 1;
        data->radius += data->vradius * env->dt;
        break;
    }
    case SHOT_CHARGED_LINEAR:
    {
        struct ChargedLinearShotData *data = (struct ChargedLinearShotData *)shot->data_;
        data->life += env->dt;
        if (data->life > data->max_life) shot->destroyed = 1;
        break;
    }
    }
}

int shot_droid_hit(Shot *shot, DroidState *droid)
{
    switch (shot->type)
    {
    case SHOT_SHELL:
    {
        float rx = shot->x - droid->x;
        float ry = shot->y - droid->y;
        int collision = vec_length(rx, ry) <= ((struct ShellShotData *)shot->data_)->radius;
        if (collision) shot->destroyed = 1;
        return collision;
    }
    case SHOT_RANGE:
    {
        // 衝突後は Range Shot の寿命が縮む <- kore ha uso
        struct RangeShotData *data = (struct RangeShotData *)shot->data_;
        int collision = vec_length(shot->x - droid->x, shot->y - droid->y) <= data->radius;
        return collision;
    }
    case SHOT_CHARGED_LINEAR:
    {
        struct ChargedLinearShotData *data = (struct ChargedLinearShotData *)shot->data_;
        // collide if abs(shot_angle - angle_of_player_to_shot) <= phi = asin (width / (2 * d))
        const float distance = vec_length(shot->x - droid->x, shot->y - droid->y);
        const float angle_of_player_to_shot = atan2f(droid->y - shot->y, droid->x - shot->x);
        const float phi = asinf(data->width / (2 * distance));
        int collision = fabsf(data->angle - angle_of_player_to_shot) <= phi;
        return collision;
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

Shot *make_range_shot(int shot_by, int attack, float x, float y, float radius, float vradius)
{
    Shot *shot = (Shot *)malloc(sizeof(Shot));
    shot->shot_by = shot_by;
    shot->attack = attack;
    shot->destroyed = 0;
    shot->type = SHOT_RANGE;
    shot->x = x;
    shot->y = y;
    shot->data_ = malloc(sizeof(struct RangeShotData));
    struct RangeShotData *data = (struct RangeShotData *)shot->data_;
    data->radius = radius;
    data->vradius = vradius;
    data->life = 0.0;
    data->max_life = 3.0;
    return shot;
}

Shot *make_charged_linear_shot(int shot_by, int attack, float x, float y, float angle, float lifespan)
{
    Shot *shot = (Shot *)malloc(sizeof(Shot));
    shot->shot_by = shot_by;
    shot->attack = attack;
    shot->destroyed = 0;
    shot->type = SHOT_CHARGED_LINEAR;
    shot->x = x;
    shot->y = y;
    shot->data_ = malloc(sizeof(struct ChargedLinearShotData));
    struct ChargedLinearShotData *data = (struct ChargedLinearShotData *)shot->data_;
    data->life = 0.0;
    data->max_life = lifespan;
    data->angle = angle;
    data->width = 15;
    return shot;
}
