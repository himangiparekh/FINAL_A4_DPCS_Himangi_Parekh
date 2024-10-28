#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "input.h"

extern Vertex origin;
extern double density;

Vertex vectorSubtract(const Vertex& a, const Vertex& b);
float vectorDot(const Vertex& a, const Vertex& b);
Vertex vectorCross(const Vertex& a, const Vertex& b);
float vectorMagnitude(const Vertex& v);
Vertex calculateCentroid(const Polyhedron& poly);
double tetrahedronVolume(const Vertex& a, const Vertex& b, const Vertex& c, const Vertex& d);
double calctetrahedronVolume(const Vertex& v1, const Vertex& v2, const Vertex& v3, const Vertex& origin);
InertiaTensor computeTetrahedronInertia(const Vertex& v1, const Vertex& v2, const Vertex& v3, const Vertex& origin, double density);

double calculatepolyhedronVolume(const Polyhedron& poly);
Vertex calculateCenterOfMass(const Polyhedron& poly);
float calculateSurfaceArea(const Polyhedron& poly);
InertiaTensor computePolyhedronInertia(const Polyhedron& poly, const Vertex& origin, double density);


#endif