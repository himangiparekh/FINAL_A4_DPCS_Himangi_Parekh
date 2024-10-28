#ifndef VALIDITY_H
#define VALIDITY_H

#include "input.h"

bool checkCollinearity(const Vertex& p1, const Vertex& p2, const Vertex& p3);
bool checkPlanarity(const std::vector<Vertex>& face);
double computeDistance(const Vertex& p1, const Vertex& p2);
bool checkEdgeLengthConsistency(const Polyhedron& poly, const std::string& polyType = "outer");
bool checkCollinearityAndPlanarity(const Polyhedron& poly, const std::string& polyType = "outer");
bool checkClosedPolyhedron(const Polyhedron& poly, const std::string& polyType = "outer");
bool validateInput(const Polyhedron& poly, const std::string& polyType = "outer");

#endif // VALIDITY_H