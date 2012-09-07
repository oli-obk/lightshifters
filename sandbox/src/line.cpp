#include "line.hpp"

Line::Line(Vector origin, Vector direction)
:origin(origin), direction(direction)
{
}

double Line::distanceSquared(Vector Q) const
{
    Vector tmp = direction.cross(Q-origin);
    return tmp.magnitudeSquared()/direction.magnitudeSquared();
}

//http://www.gamedev.net/topic/198199-distance-from-point-to-line-segment/
// TODO: can this be improved for speed?
double Line::segmentDistanceSquared(Vector point) const
{
	Vector c = point - origin;	// Vector from a to Point
	Vector v = direction.normalized();	// Unit direction Vector
	float d = direction.magnitude();	// Length of the line segment
	float t = v.dot(c);	// Intersection point Distance from origin

	// Check to see if the point is on the line
	// if not then return the endpoint
	if(t < 0) return c.magnitudeSquared();
	if(t > d) return (c+direction).magnitudeSquared();

	// get the distance to move from point a
	v *= t;

	// move from point a to the nearest point on the segment
	return (c - v).magnitudeSquared();
}
