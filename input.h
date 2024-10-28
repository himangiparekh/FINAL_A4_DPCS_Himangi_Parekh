#ifndef INPUT_H
#define INPUT_H

#include "constants.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <cstring>
#include <SDL2/SDL.h>
#include <Eigen/Dense>
#include <map>
#include <algorithm> 
#include <limits>
#include <utility>
#include <cstdio>
#include <string>

using namespace std;
using namespace Eigen;

struct Vertex {
    double x, y, z;

    // Define the '<' operator to compare Vertex objects
    bool operator<(const Vertex& other) const {
        if (x != other.x) return x < other.x;
        if (y != other.y) return y < other.y;
        return z < other.z;
    }
};

struct Edge {
    Vertex v1, v2;
    double length;
};

struct Face {
    vector<Edge> edges;
    int num_edges; 
};

struct Polyhedron {
    vector<Face> faces;
    vector<Vertex> vertices;
    vector<Polyhedron> sub_polyhedrons; // Stores internal "hole" polyhedrons
};

// Struct to hold the inertia tensor values
struct InertiaTensor {
    double Ixx, Iyy, Izz, Ixy, Ixz, Iyz;

    InertiaTensor() : Ixx(0), Iyy(0), Izz(0), Ixy(0), Ixz(0), Iyz(0) {}

    // Add tensor contributions
    InertiaTensor& operator+=(const InertiaTensor& other) {
        Ixx += other.Ixx; Iyy += other.Iyy; Izz += other.Izz;
        Ixy += other.Ixy; Ixz += other.Ixz; Iyz += other.Iyz;
        return *this;
    }

    // Subtract tensor contributions (for holes)
    InertiaTensor& operator-=(const InertiaTensor& other) {
        Ixx -= other.Ixx; Iyy -= other.Iyy; Izz -= other.Izz;
        Ixy -= other.Ixy; Ixz -= other.Ixz; Iyz -= other.Iyz;
        return *this;
    }
};

void getInput(Polyhedron& poly, const string& polyType = "outer");

void printPolyhedron(const Polyhedron& poly, const string& polyType = "outer", int level = 1);

#endif 
