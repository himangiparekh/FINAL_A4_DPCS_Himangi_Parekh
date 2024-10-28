#ifndef PROJECTIONS_H
#define PROJECTIONS_H

#include "input.h"

void orthographicProjectionCustomPlane(const Polyhedron& poly, float A, float B, float C, float D);
SDL_Point projectTo2D(Vertex v, float angleX, float angleY);
void drawPolyhedron(SDL_Renderer* renderer, const Polyhedron& polyhedron, float angleX, float angleY, SDL_Color color);

#endif