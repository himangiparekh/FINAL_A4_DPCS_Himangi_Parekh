#include "validity.h"

using namespace std;
using namespace Eigen;

// Helper function to check collinearity of three points
bool checkCollinearity(const Vertex& p1, const Vertex& p2, const Vertex& p3) {
    Vector3d v1(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
    Vector3d v2(p3.x - p2.x, p3.y - p2.y, p3.z - p2.z);
    Vector3d crossProduct = v1.cross(v2);
    return crossProduct.norm() < 1e-6;  // If the cross product is near zero, points are collinear
}

bool checkPlanarity(const vector<Vertex>& face) {
    if (face.size() < 3) return true;  // Less than 3 points are always planar

    // Calculate the normal vector from the first 3 points
    Vector3d v1(face[1].x - face[0].x, face[1].y - face[0].y, face[1].z - face[0].z);
    Vector3d v2(face[2].x - face[0].x, face[2].y - face[0].y, face[2].z - face[0].z);
    Vector3d normal = v1.cross(v2);

    if (normal.norm() < 1e-6) return false;  // Degenerate normal, points are likely collinear

    // Check if all other points lie on the same plane
    for (size_t i = 3; i < face.size(); ++i) {
        Vector3d v(face[i].x - face[0].x, face[i].y - face[0].y, face[i].z - face[0].z);
        if (fabs(normal.dot(v)) > 1e-6) {  // Dot product should be close to zero for planarity
            return false;  // Not planar
        }
    }

    return true;  // All points are planar
}

// Helper function to compute distance between two points
double computeDistance(const Vertex& p1, const Vertex& p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2) + pow(p1.z - p2.z, 2));
}

bool checkEdgeLengthConsistency(const Polyhedron& poly, const std::string& polyType) {
    for (size_t i = 0; i < poly.faces.size(); i++) {
        for (size_t j = 0; j < poly.faces[i].edges.size(); j++) {
            const Edge& edge = poly.faces[i].edges[j];
            double dist3D = computeDistance(edge.v1, edge.v2);
            if (std::fabs(dist3D - edge.length) > 1e-6) {
                std::printf("Edge length inconsistency detected for edge %zu in face %zu of the %s polyhedron\n", j + 1, i + 1, polyType.c_str());
                return false;
            }
        }
    }
    return true;
}

bool checkCollinearityAndPlanarity(const Polyhedron& poly, const std::string& polyType) {
    for (size_t i = 0; i < poly.faces.size(); i++) {
        std::vector<Vertex> faceVertices;
        for (size_t j = 0; j < poly.faces[i].edges.size(); j++) {
            faceVertices.push_back(poly.faces[i].edges[j].v1);
        }

        if (faceVertices.size() >= 3) {
            for (size_t k = 0; k < faceVertices.size() - 2; ++k) {
                if (checkCollinearity(faceVertices[k], faceVertices[k + 1], faceVertices[k + 2])) {
                    std::printf("Collinearity detected for points in face %zu of the %s polyhedron\n", i + 1, polyType.c_str());
                    return false;
                }
            }
        }

        if (!checkPlanarity(faceVertices)) {
            std::printf("Non-planar face detected for face %zu of the %s polyhedron\n", i + 1, polyType.c_str());
            return false;
        }
    }
    return true;
}

bool checkClosedPolyhedron(const Polyhedron& poly, const std::string& polyType) {
    std::map<std::pair<Vertex, Vertex>, int> edgeCount;
    for (const auto& face : poly.faces) {
        for (const auto& edge : face.edges) {
            std::pair<Vertex, Vertex> edgeKey = std::minmax(edge.v1, edge.v2);
            edgeCount[edgeKey]++;
        }
    }

    for (const auto& entry : edgeCount) {
        if (entry.second != 2) {
            std::printf("Edge between vertices (%.2f, %.2f, %.2f) and (%.2f, %.2f, %.2f) is not shared by exactly two faces in the %s polyhedron\n",
                entry.first.first.x, entry.first.first.y, entry.first.first.z,
                entry.first.second.x, entry.first.second.y, entry.first.second.z, polyType.c_str());
            return false;
        }
    }
    return true;
}

// Master validation function
bool validateInput(const Polyhedron& poly, const std::string& polyType) {
    if (!checkEdgeLengthConsistency(poly, polyType)) {
        return false;
    }

    if (!checkCollinearityAndPlanarity(poly, polyType)) {
        return false;
    }

    if (!checkClosedPolyhedron(poly, polyType)) {
        return false;
    }

    for (size_t i = 0; i < poly.sub_polyhedrons.size(); ++i) {
        std::string holeType = polyType + " internal hole " + std::to_string(i + 1);
        if (!validateInput(poly.sub_polyhedrons[i], holeType)) {
            return false; // Stop if any sub-polyhedron validation fails
        }
    }

    return true;
}

