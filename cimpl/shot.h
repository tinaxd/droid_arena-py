#pragma once

#include "droid.h"
#include "env.h"

typedef enum
{
    SHOT_SHELL,
    SHOT_RANGE,
    SHOT_CHARGED_LINEAR,
} ShotType;

struct ShellShotData
{
    float vx, vy;
    float life;
    float maxlife;
    float radius;
};

struct RangeShotData
{
    float radius;
    float vradius;
    float max_life;
    float life;
};

struct ChargedLinearShotData
{
    float max_life;
    float life;
    float angle;
    float width;
};

struct Shot
{
    ShotType type;
    float x, y;
    int destroyed;
    int attack;
    int shot_by;
    void *data_;
};

void free_shot(Shot *shot);

void shot_process(Shot *shot, struct Environment *env);
int shot_droid_hit(Shot *shot, DroidState *droid);

Shot *make_shell_shot(int shot_by, int attack, float x, float y, float vx, float vy, float radius);
Shot *make_range_shot(int shot_by, int attack, float x, float y, float radius, float vradius);

// angle must be [-pi pi]
Shot *make_charged_linear_shot(int shot_by, int attack, float x, float y, float angle, float lifespan);
