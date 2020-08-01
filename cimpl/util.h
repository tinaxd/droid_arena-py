#pragma once

#include <math.h>

typedef struct {
    short r;
    short g;
    short b;
    short a;
} ColorRGBA;

static ColorRGBA color_complement(ColorRGBA color)
{
    ColorRGBA c;
    c.r = 255 - color.r;
    c.g = 255 - color.g;
    c.b = 255 - color.b;
    c.a = color.a;
    return c;
}

static float vec_length(float x, float y) {
    return sqrt(x * x + y * y);
}

static float vec_dot(float x1, float y1, float x2, float y2) {
    return x1 * x2 + y1 * y2;
}
