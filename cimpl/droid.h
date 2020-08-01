#pragma once

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "util.h"

struct Environment;
typedef struct Shot Shot;

typedef enum
{
    DROID_CMD_MOVE_F,
    DROID_CMD_MOVE_B,
    DROID_CMD_TURN_L,
    DROID_CMD_TURN_R,
    DROID_CMD_TURN_ENEMY,
    DROID_CMD_SHOT_SHELL,
} DroidCommand;

typedef struct
{
    size_t n;
    DroidCommand *cmds;
    size_t cmd_index_;
    float cmd_exec_time_;
    float cmd_timeout_;
    int cmd_executed_;
} DroidCommandDriver;

DroidCommandDriver *make_cmd_driver(size_t n, const DroidCommand *cmds);
void free_cmd_driver(DroidCommandDriver *driver);

typedef enum
{
    DROID_COMMAND,
    DROID_SCRIPTED,
} DroidType;

typedef struct
{
    char *id;
    int team;
    float x;
    float y;
    float rot;
    int hp;
    int destroyed;
    ColorRGBA color;
    DroidType type;
    void *driver;
} DroidState;

DroidState *make_droid(const char *id, int team, float x, float y, float rot, int hp, ColorRGBA color, DroidType ty);
void free_droid(DroidState *droid);

void front_vector(const DroidState *droid, float *x, float *y);

void next_command(DroidState *droid, struct Environment *env);

void droid_hit(DroidState *droid, Shot *shot);
