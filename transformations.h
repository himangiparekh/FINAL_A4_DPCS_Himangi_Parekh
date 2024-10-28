#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include "input.h"
#include "validity.h"

void rotate_point(Vertex *p, double angle, double A, double B, double C);
void translate_point(Vertex *p, double dx, double dy, double dz);
void scale_point(Vertex *p, double sx, double sy, double sz);
void reflect_point(Vertex *p, double A, double B, double C, double D);

void rotate_polyhedron(Polyhedron &poly, double angle, double A, double B, double C);
void translate_polyhedron(Polyhedron &poly, double dx, double dy, double dz);
void scale_polyhedron(Polyhedron &poly, double sx, double sy, double sz);
void reflect_polyhedron(Polyhedron &poly, double A, double B, double C, double D);

bool getValidatedDouble(double &value, const std::string &prompt);
bool getValidatedFloat(float &value, const std::string &prompt);
bool getValidatedChoice(int &choice, int min, int max, const std::string &prompt);
Polyhedron deep_copy(const Polyhedron &poly);
void update_edges(Polyhedron& poly);

void transform_polyhedron(const Polyhedron &poly);

#endif