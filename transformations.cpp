#include "transformations.h"

using namespace std;
using namespace Eigen;

// Rotate a single vertex around a plane's normal vector
void rotate_point(Vertex *p, double angle, double A, double B, double C) {
    double rad = angle * M_PI / 180.0;
    double c = cos(rad);
    double s = sin(rad);
    double t = 1.0 - c;

    // Normalize the normal vector (A, B, C)
    double magnitude = sqrt(A * A + B * B + C * C);
    double nx = A / magnitude, ny = B / magnitude, nz = C / magnitude;

    // Compute rotation matrix components based on normal vector
    double m11 = t * nx * nx + c;
    double m12 = t * nx * ny - s * nz;
    double m13 = t * nx * nz + s * ny;
    double m21 = t * nx * ny + s * nz;
    double m22 = t * ny * ny + c;
    double m23 = t * ny * nz - s * nx;
    double m31 = t * nx * nz - s * ny;
    double m32 = t * ny * nz + s * nx;
    double m33 = t * nz * nz + c;

    // Apply rotation matrix to point
    double x = p->x, y = p->y, z = p->z;
    p->x = m11 * x + m12 * y + m13 * z;
    p->y = m21 * x + m22 * y + m23 * z;
    p->z = m31 * x + m32 * y + m33 * z;
}

// Translate a single vertex
void translate_point(Vertex *p, double dx, double dy, double dz) {
    p->x += dx;
    p->y += dy;
    p->z += dz;
}

// Scale a single vertex
void scale_point(Vertex *p, double sx, double sy, double sz) {
    p->x *= sx;
    p->y *= sy;
    p->z *= sz;
}

// Reflect a single vertex across a plane Ax + By + Cz = D
void reflect_point(Vertex *p, double A, double B, double C, double D) {
    // Normalize the normal vector (A, B, C)
    double magnitude = sqrt(A * A + B * B + C * C);
    double nx = A / magnitude, ny = B / magnitude, nz = C / magnitude;
    
    // Distance from point to plane
    double dist = (A * p->x + B * p->y + C * p->z - D) / magnitude;
    
    // Reflect point across the plane
    p->x -= 2 * dist * nx;
    p->y -= 2 * dist * ny;
    p->z -= 2 * dist * nz;
}

// Rotate all vertices in a polyhedron (including sub-polyhedrons) around a plane normal
void rotate_polyhedron(Polyhedron &poly, double angle, double A, double B, double C) {
    for (auto &vertex : poly.vertices) {
        rotate_point(&vertex, angle, A, B, C);
    }
    for (auto &sub_poly : poly.sub_polyhedrons) {
        rotate_polyhedron(sub_poly, angle, A, B, C);
    }
}

// Helper function to apply a translation to all vertices in a polyhedron (including sub-polyhedrons)
void translate_polyhedron(Polyhedron &poly, double dx, double dy, double dz) {
    for (auto &vertex : poly.vertices) {
        translate_point(&vertex, dx, dy, dz);
    }
    for (auto &sub_poly : poly.sub_polyhedrons) {
        translate_polyhedron(sub_poly, dx, dy, dz);
    }
}

// Helper function to apply scaling to all vertices in a polyhedron (including sub-polyhedrons)
void scale_polyhedron(Polyhedron &poly, double sx, double sy, double sz) {
    for (auto &vertex : poly.vertices) {
        scale_point(&vertex, sx, sy, sz);
    }
    for (auto &sub_poly : poly.sub_polyhedrons) {
        scale_polyhedron(sub_poly, sx, sy, sz);
    }
}

// Reflect all vertices in a polyhedron (including sub-polyhedrons) across a plane
void reflect_polyhedron(Polyhedron &poly, double A, double B, double C, double D) {
    for (auto &vertex : poly.vertices) {
        reflect_point(&vertex, A, B, C, D);
    }
    for (auto &sub_poly : poly.sub_polyhedrons) {
        reflect_polyhedron(sub_poly, A, B, C, D);
    }
}

bool getValidatedDouble(double &value, const std::string &prompt) {
    std::cout << prompt;
    while (!(std::cin >> value)) {
        std::cout << "Invalid input. Please enter a number.\n" << prompt;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return true;
}

bool getValidatedFloat(float &value, const std::string &prompt) {
    std::cout << prompt;
    while (!(std::cin >> value)) {
        std::cout << "Invalid input. Please enter a number.\n" << prompt;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return true;
}

bool getValidatedChoice(int &choice, int min, int max, const std::string &prompt) {
    std::cout << prompt;
    while (!(std::cin >> choice) || choice < min || choice > max) {
        std::cout << "Invalid input. Please enter a number between " << min << " and " << max << ".\n" << prompt;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return true;
}

Polyhedron deep_copy(const Polyhedron &poly) {
    Polyhedron copy;
    copy.vertices = poly.vertices; // Copy vertices

    // Recursively deep copy sub-polyhedrons
    for (const auto &sub_poly : poly.sub_polyhedrons) {
        copy.sub_polyhedrons.push_back(deep_copy(sub_poly));
    }
    return copy;
}

void update_edges(Polyhedron& poly) {
    for (auto& face : poly.faces) {
        for (auto& edge : face.edges) {
            // Directly use the vertices from the edge
            edge.length = computeDistance(edge.v1, edge.v2); // Recalculate length based on updated vertices
        }
    }

    // Recursively update edges for any sub-polyhedrons
    for (auto& sub_poly : poly.sub_polyhedrons) {
        update_edges(sub_poly);
    }
}

void transform_polyhedron(const Polyhedron &poly) {
    // Clone poly so the original is unchanged
    Polyhedron poly_copy = deep_copy(poly);

    int choice;
    getValidatedChoice(choice, 1, 4, "Choose transformation:\n1. Rotate around a plane\n2. Translate\n3. Scale\n4. Reflect across a plane\n");

    switch (choice) {
        case 1: { // Rotate
            double angle;
            float A, B, C, D;
            getValidatedDouble(angle, "Enter the rotation angle (in degrees): ");
            
            std::cout << "Enter the coefficients of the plane equation (Ax + By + Cz = D):\n";
            getValidatedFloat(A, "A (normal x-component): ");
            getValidatedFloat(B, "B (normal y-component): ");
            getValidatedFloat(C, "C (normal z-component): ");
            getValidatedFloat(D, "D (distance from origin): ");

            if (A == 0 && B == 0 && C == 0) {
                std::cout << "Error: The normal vector cannot be zero. Rotation canceled.\n";
                return;
            }

            rotate_polyhedron(poly_copy, angle, A, B, C);
            std::cout << "\nRotated Polyhedron:\n";
            update_edges(poly_copy);
            printPolyhedron(poly_copy);
            break;
        }
        case 2: { // Translate
            double dx, dy, dz;
            getValidatedDouble(dx, "Enter translation value dx: ");
            getValidatedDouble(dy, "Enter translation value dy: ");
            getValidatedDouble(dz, "Enter translation value dz: ");

            translate_polyhedron(poly_copy, dx, dy, dz);
            std::cout << "\nTranslated Polyhedron:\n";
            update_edges(poly_copy);
            printPolyhedron(poly_copy);
            break;
        }
        case 3: { // Scale
            double sx, sy, sz;
            getValidatedDouble(sx, "Enter scaling factor sx: ");
            getValidatedDouble(sy, "Enter scaling factor sy: ");
            getValidatedDouble(sz, "Enter scaling factor sz: ");

            scale_polyhedron(poly_copy, sx, sy, sz);
            std::cout << "\nScaled Polyhedron:\n";
            update_edges(poly_copy);
            printPolyhedron(poly_copy);
            break;
        }
        case 4: { // Reflect
            float A, B, C, D;
            std::cout << "Enter the coefficients of the plane equation (Ax + By + Cz = D):\n";
            getValidatedFloat(A, "A (normal x-component): ");
            getValidatedFloat(B, "B (normal y-component): ");
            getValidatedFloat(C, "C (normal z-component): ");
            getValidatedFloat(D, "D (distance from origin): ");

            if (A == 0 && B == 0 && C == 0) {
                std::cout << "Error: The normal vector cannot be zero. Reflection canceled.\n";
                return;
            }

            reflect_polyhedron(poly_copy, A, B, C, D);
            std::cout << "\nReflected Polyhedron:\n";
            update_edges(poly_copy);
            printPolyhedron(poly_copy);
            break;
        }
        default:
            std::cout << "Invalid choice\n";
    }
}