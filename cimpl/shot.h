#pragma once

#include "droid.h"
#include "env.h"

typedef enum
{
    SHOT_SHELL,
} ShotType;

struct ShellShotData
{
    float vx, vy;
    float life;
    float maxlife;
    float radius;
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
int shot_droid_hit(const Shot *shot, const DroidState *droid);

Shot *make_shell_shot(int shot_by, int attack, float x, float y, float vx, float vy, float radius);
