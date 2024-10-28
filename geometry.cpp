#include "geometry.h"

using namespace std;
using namespace Eigen;


Vertex vectorSubtract(const Vertex& a, const Vertex& b) { // const ensures that the input arguments a and b are read-only within the function
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

float vectorDot(const Vertex& a, const Vertex& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vertex vectorCross(const Vertex& a, const Vertex& b) { // cross product
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

float vectorMagnitude(const Vertex& v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

float calculateSurfaceArea(const Polyhedron& poly) {
    float totalArea = 0.0f;

    // Loop through the faces of the outer polyhedron only
    for (const auto& face : poly.faces) {
        // Ensure the face has at least 3 edges to form a surface
        if (face.edges.size() < 3) continue;

        Vertex v0 = face.edges[0].v1; // Reference vertex for triangles

        for (size_t j = 1; j < face.edges.size() - 1; j++) {
            Vertex v1 = face.edges[j].v1;
            Vertex v2 = face.edges[j + 1].v1;

            // Calculate vectors for two edges of the triangle
            Vertex edge1 = vectorSubtract(v1, v0);
            Vertex edge2 = vectorSubtract(v2, v0);

            // Cross product to find the area of the triangle
            Vertex crossProd = vectorCross(edge1, edge2);
            float triangleArea = vectorMagnitude(crossProd) / 2.0f;

            totalArea += triangleArea;
        }
    }

    return totalArea;
}

Vertex calculateCentroid(const Polyhedron& poly) {
    Vertex centroid = {0, 0, 0};
    for (const Vertex& v : poly.vertices) {
        centroid.x += v.x;
        centroid.y += v.y;
        centroid.z += v.z;
    }
    double numVertices = poly.vertices.size();
    centroid.x /= numVertices;
    centroid.y /= numVertices;
    centroid.z /= numVertices;
    return centroid;
}

// Helper function to calculate the volume of a tetrahedron
double tetrahedronVolume(const Vertex& a, const Vertex& b, const Vertex& c, const Vertex& d) {
    double volume = (a.x - d.x) * ((b.y - d.y) * (c.z - d.z) - (c.y - d.y) * (b.z - d.z))
                  - (a.y - d.y) * ((b.x - d.x) * (c.z - d.z) - (c.x - d.x) * (b.z - d.z))
                  + (a.z - d.z) * ((b.x - d.x) * (c.y - d.y) - (c.x - d.x) * (b.y - d.y));
    return fabs(volume) / 6.0;
}

double calculatepolyhedronVolume(const Polyhedron& poly) {
    double volume = 0.0;
    Vertex centroid = calculateCentroid(poly);
    
    // Decompose each face into tetrahedrons using the centroid
    for (const Face& face : poly.faces) {
        const Vertex& v1 = face.edges[0].v1;
        for (size_t i = 1; i < face.edges.size() - 1; ++i) {
            const Vertex& v2 = face.edges[i].v1;
            const Vertex& v3 = face.edges[i + 1].v1;
            volume += tetrahedronVolume(centroid, v1, v2, v3);
        }
    }
    
    // Subtract volumes of sub-polyhedrons
    for (const Polyhedron& subPoly : poly.sub_polyhedrons) {
        volume -= calculatepolyhedronVolume(subPoly);
    }
    
    return volume;
}

// Function to calculate the centroid of a tetrahedron
Vertex calculateTetrahedronCentroid(const Vertex& v0, const Vertex& v1, const Vertex& v2) {
    return {
        (v0.x + v1.x + v2.x) / 4.0f,
        (v0.y + v1.y + v2.y) / 4.0f,
        (v0.z + v1.z + v2.z) / 4.0f
    };
}

float calculateTetrahedronVolume(const Vertex& origin, const Vertex& v1, const Vertex& v2, const Vertex& v3) {
    // Calculate volume of a tetrahedron formed by origin and the three vertices
    Vertex v1_rel = vectorSubtract(v1, origin);
    Vertex v2_rel = vectorSubtract(v2, origin);
    Vertex v3_rel = vectorSubtract(v3, origin);

    // Cross product of v2_rel and v3_rel, then dot product with v1_rel
    return std::abs(vectorDot(v1_rel, vectorCross(v2_rel, v3_rel))) / 6.0f;
}

// Function to calculate the volume and weighted center of mass for a polyhedron
float calculatePolyhedronVolumeAndCenter(const Polyhedron& poly, Vertex& weightedCenter) {
    float totalVolume = 0.0f;
    weightedCenter = {0, 0, 0};

    for (const auto& face : poly.faces) {
        if (face.edges.size() < 3) continue; // Ignore degenerate faces

        Vertex v0 = face.edges[0].v1;

        for (size_t j = 1; j < face.edges.size() - 1; j++) {
            Vertex v1 = face.edges[j].v1;
            Vertex v2 = face.edges[j + 1].v1;

            float tetrahedronVolume = calculateTetrahedronVolume(origin, v0, v1, v2);
            Vertex tetrahedronCentroid = calculateTetrahedronCentroid(v0, v1, v2);

            // Accumulate volume-weighted centroid
            weightedCenter.x += tetrahedronCentroid.x * tetrahedronVolume;
            weightedCenter.y += tetrahedronCentroid.y * tetrahedronVolume;
            weightedCenter.z += tetrahedronCentroid.z * tetrahedronVolume;

            totalVolume += tetrahedronVolume;
        }
    }

    return totalVolume;
}

Vertex calculateCenterOfMass(const Polyhedron& poly) {
    Vertex outerWeightedCenter = {0, 0, 0};
    float outerVolume = calculatePolyhedronVolumeAndCenter(poly, outerWeightedCenter);

    float totalVolume = outerVolume;
    Vertex totalWeightedCenter = outerWeightedCenter;

    // Subtract the contribution of each sub-polyhedron (hole)
    for (const auto& hole : poly.sub_polyhedrons) {
        Vertex holeWeightedCenter = {0, 0, 0};
        float holeVolume = calculatePolyhedronVolumeAndCenter(hole, holeWeightedCenter);

        // Adjust the total volume and weighted center to "subtract" the hole's effect
        totalVolume -= holeVolume;
        totalWeightedCenter.x -= holeWeightedCenter.x;
        totalWeightedCenter.y -= holeWeightedCenter.y;
        totalWeightedCenter.z -= holeWeightedCenter.z;
    }

    // Compute the final center of mass by dividing the weighted sum by the total volume
    Vertex centerOfMass = {0, 0, 0};
    if (totalVolume > 0) {
        centerOfMass.x = totalWeightedCenter.x / totalVolume;
        centerOfMass.y = totalWeightedCenter.y / totalVolume;
        centerOfMass.z = totalWeightedCenter.z / totalVolume;

        // Round values close to zero within EPSILON to zero
        if (std::fabs(centerOfMass.x) < EPSILON) centerOfMass.x = 0;
        if (std::fabs(centerOfMass.y) < EPSILON) centerOfMass.y = 0;
        if (std::fabs(centerOfMass.z) < EPSILON) centerOfMass.z = 0;
    }

    return centerOfMass;
}

// Helper function to compute the volume of a tetrahedron
double calctetrahedronVolume(const Vertex& v1, const Vertex& v2, const Vertex& v3, const Vertex& origin) {
    return fabs((v1.x - origin.x) * ((v2.y - origin.y) * (v3.z - origin.z) - (v2.z - origin.z) * (v3.y - origin.y)) -
                (v1.y - origin.y) * ((v2.x - origin.x) * (v3.z - origin.z) - (v2.z - origin.z) * (v3.x - origin.x)) +
                (v1.z - origin.z) * ((v2.x - origin.x) * (v3.y - origin.y) - (v2.y - origin.y) * (v3.x - origin.x))) / 6.0;
}

// Helper function to compute the inertia tensor of a single tetrahedron
InertiaTensor computeTetrahedronInertia(const Vertex& v1, const Vertex& v2, const Vertex& v3, const Vertex& origin, double density) {
    InertiaTensor tensor;

    // Compute the volume of the tetrahedron
    double volume = calctetrahedronVolume(v1, v2, v3, origin);
    double mass = density * volume;

    // Compute inertia tensor components for the tetrahedron
    double x1 = v1.x - origin.x, y1 = v1.y - origin.y, z1 = v1.z - origin.z;
    double x2 = v2.x - origin.x, y2 = v2.y - origin.y, z2 = v2.z - origin.z;
    double x3 = v3.x - origin.x, y3 = v3.y - origin.y, z3 = v3.z - origin.z;

    tensor.Ixx = mass * (y1 * y1 + z1 * z1 + y2 * y2 + z2 * z2 + y3 * y3 + z3 * z3) / 10.0;
    tensor.Iyy = mass * (x1 * x1 + z1 * z1 + x2 * x2 + z2 * z2 + x3 * x3 + z3 * z3) / 10.0;
    tensor.Izz = mass * (x1 * x1 + y1 * y1 + x2 * x2 + y2 * y2 + x3 * x3 + y3 * y3) / 10.0;

    tensor.Ixy = -mass * (x1 * y1 + x2 * y2 + x3 * y3) / 10.0;
    tensor.Ixz = -mass * (x1 * z1 + x2 * z2 + x3 * z3) / 10.0;
    tensor.Iyz = -mass * (y1 * z1 + y2 * z2 + y3 * z3) / 10.0;

    return tensor;
}

// Function to compute moment of inertia for a polyhedron
InertiaTensor computePolyhedronInertia(const Polyhedron& poly, const Vertex& origin, double density) {
    InertiaTensor total_inertia;

    // Compute the inertia of the main polyhedron
    for (const Face& face : poly.faces) {
        // Assuming the face is already triangulated
        for (size_t i = 1; i < face.edges.size() - 1; ++i) {
            const Vertex& v1 = face.edges[0].v1;
            const Vertex& v2 = face.edges[i].v2;
            const Vertex& v3 = face.edges[i + 1].v2;

            total_inertia += computeTetrahedronInertia(v1, v2, v3, origin, density);
        }
    }

    // Subtract the inertia of each hole
    for (const Polyhedron& hole : poly.sub_polyhedrons) {
        total_inertia -= computePolyhedronInertia(hole, origin, density);
    }

    return total_inertia;
}