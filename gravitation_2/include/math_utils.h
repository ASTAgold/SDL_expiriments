#include <math.h>
#include "vector2d.h"

float remap(float value, float inputStart, float inputEnd, float outputStart, float outputEnd)
{
    float result = (value - inputStart)/(inputEnd - inputStart)*(outputEnd - outputStart) + outputStart;

    return result;
}

Vector2D Vector2Clamp(Vector2D v, Vector2D min, Vector2D max)
{
    Vector2D result;

    result.setX(fminf(max.getX(), fmaxf(min.getX(), v.getX())));
    result.setY(fminf(max.getY(), fmaxf(min.getY(), v.getY())));

    return result;
}