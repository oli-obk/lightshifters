#ifndef LINE_HPP
#define LINE_HPP

#include "Vector.h"

class Line
{
public:
    Line(Vector origin, Vector direction);
    Vector origin;
    Vector direction;
    double distanceSquared(Vector point) const;
    // interpret this as a segment from origin to (origin+direction)
    double segmentDistanceSquared(Vector point) const;
};

#endif // LINE_HPP
