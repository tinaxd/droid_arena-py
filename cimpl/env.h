#pragma once

#include "droid.h"


struct Environment {
    float dt;
    size_t n_droids_;
    DroidState **droids_;

    size_t n_shots_queue_;
    
};

DroidState *env_query_nearest_enemy(const struct Environment *env, const DroidState *droid);
void env_new_shot(struct Environment *env);
