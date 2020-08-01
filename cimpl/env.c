#include "env.h"
#include <float.h>

DroidState *env_query_nearest_enemy(const struct Environment *env, const DroidState *self)
{
    DroidState *target = NULL;
    float distance = MAXFLOAT;
    for (size_t i = 0; i < env->n_droids_; i++)
    {
        DroidState *droid = env->droids_[i];
        if (droid->team == self->team)
            continue;
        float dist = vec_length(droid->x - self->x, droid->y - self->y);
        if (dist < distance)
        {
            distance = dist;
            target = droid;
        }
    }
    return target;
}

void env_new_shot(struct Environment *env, Shot *shot)
{
    if (env->n_shots_queue_ >= env->c_shots_queue_) {
        env->c_shots_queue_ = (env->c_shots_queue_ + 1) * 2;
        env->shots_queue_ = (Shot **)realloc(env->shots_queue_, sizeof(Shot *) * env->c_shots_queue_);
        assert(env->shots_queue_ != NULL);
    }

    env->shots_queue_[env->n_shots_queue_++] = shot;
}
