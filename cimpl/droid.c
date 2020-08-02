#include "droid.h"
#include "env.h"
#include "shot.h"

DroidState *make_droid(const char *id, int team, float x, float y, float rot, int hp, ColorRGBA color, DroidType ty)
{
    DroidState *droid = (DroidState *)malloc(sizeof(DroidState));
    assert(droid != NULL);

    droid->id = (char *)malloc(strlen(id) + 1);
    strcpy(droid->id, id);
    droid->team = team;
    droid->x = x;
    droid->y = y;
    droid->rot = rot;
    droid->hp = hp;
    droid->maxhp = hp;
    droid->destroyed = 0;
    droid->color = color;
    droid->type = ty;
    return droid;
}

void free_droid(DroidState *droid)
{
    free(droid->id);
    droid->id = NULL;
}

void front_vector(const DroidState *droid, float *x, float *y)
{
    *x = cos(droid->rot);
    *y = sin(droid->rot);
}

DroidCommandDriver *make_cmd_driver(size_t n, const DroidCommand *cmds)
{
    DroidCommandDriver *drv = (DroidCommandDriver *)malloc(sizeof(DroidCommandDriver));
    assert(drv != NULL);

    drv->n = n;
    drv->cmds = (DroidCommand *)malloc(sizeof(DroidCommand) * n);
    memcpy(drv->cmds, cmds, sizeof(DroidCommand) * n);
    drv->cmd_index_ = 0;
    drv->cmd_exec_time_ = 0.0;
    drv->cmd_timeout_ = 1.0;
    drv->cmd_executed_ = 0;
    return drv;
}

void free_cmd_driver(DroidCommandDriver *driver)
{
    free(driver->cmds);
    driver->cmds = NULL;
}

void next_command_command_drv_(DroidState *droid, struct Environment *env);

void next_command(DroidState *droid, struct Environment *env)
{
    switch (droid->type)
    {
    case DROID_COMMAND:
        next_command_command_drv_(droid, env);
        break;
    case DROID_SCRIPTED:
        assert(0);
        break;
    }
}

void command_drv_update_command(DroidCommandDriver *drv, const struct Environment *env)
{
    drv->cmd_exec_time_ += env->dt;
    if (drv->cmd_exec_time_ > drv->cmd_timeout_)
    {
        drv->cmd_executed_ = 0;
        drv->cmd_exec_time_ = 0.0;
        drv->cmd_index_ += 1;
        if (drv->cmd_index_ >= drv->n)
        {
            drv->cmd_index_ = 0;
        }
    }
}

void next_command_command_drv_(DroidState *droid, struct Environment *env)
{
    DroidCommandDriver *drv = (DroidCommandDriver *)droid->driver;

    command_drv_update_command(drv, env);
    float fx, fy;
    front_vector(droid, &fx, &fy);
    switch (drv->cmds[drv->cmd_index_])
    {
    case DROID_CMD_MOVE_F:
        droid->x += fx * env->dt * 30.0;
        droid->y += fy * env->dt * 30.0;
        break;
    case DROID_CMD_MOVE_B:
        droid->x -= fx * env->dt * 30.0;
        droid->y -= fy * env->dt * 30.0;
        break;
    case DROID_CMD_TURN_L:
        droid->rot -= M_PI / 2.0 / drv->cmd_timeout_ * env->dt;
        break;
    case DROID_CMD_TURN_R:
        droid->rot += M_PI / 2.0 / drv->cmd_timeout_ * env->dt;
        break;
    case DROID_CMD_TURN_ENEMY:
    {
        DroidState *target = env_query_nearest_enemy(env, droid);
        float rx = target->x - droid->x;
        float ry = target->y - droid->y;
        float angle = acosf(vec_dot(fx, fy, rx, ry) / vec_length(rx, ry));
        droid->rot -= angle / drv->cmd_timeout_ * env->dt;
        break;
    }
    case DROID_CMD_SHOT_SHELL:
    {
        if (!drv->cmd_executed_) {
            env_new_shot(env, make_shell_shot(droid->team, 5, droid->x, droid->y, fx * 100, fy * 100, 30));
            drv->cmd_executed_ = 1;
        }
        break;
    }
    }
}

void droid_hit(DroidState *droid, Shot *shot)
{
    droid->hp -= shot->attack;
    if (droid->hp <= 0)
    {
        droid->destroyed = 1;
    }
}
