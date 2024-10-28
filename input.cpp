#include "input.h"

using namespace std;
using namespace Eigen;

// Recursive function to get input for a polyhedron and its internal holes
void getInput(Polyhedron& poly, const string& polyType) {
    int numVertices, numFaces;
    
    printf("Enter the number of vertices in the %s polyhedron: ", polyType.c_str());
    scanf("%d", &numVertices);

    printf("Enter the number of faces in the %s polyhedron: ", polyType.c_str());
    scanf("%d", &numFaces);
    poly.faces.resize(numFaces);

    // Get 2D projections for each vertex of the current polyhedron
    vector<double> xy(numVertices * 2);
    vector<double> xz(numVertices * 2);

    printf("Enter the 2D coordinates for the vertices in the %s polyhedron:\n", polyType.c_str());
    for (int i = 0; i < numVertices; ++i) {
        printf("Vertex %d (xy): ", i + 1);
        scanf("%lf %lf", &xy[i * 2], &xy[i * 2 + 1]);

        printf("Vertex %d (xz): ", i + 1);
        scanf("%lf %lf", &xz[i * 2], &xz[i * 2 + 1]);
    }

    // Reconstruct 3D vertices for this polyhedron
    poly.vertices.resize(numVertices);
    MatrixXd A(4, 3);
    A << 1, 0, 0,
         0, 1, 0,
         1, 0, 0,
         0, 0, 1;

    for (int i = 0; i < numVertices; ++i) {
        VectorXd B(4);
        B << xy[i * 2], xy[i * 2 + 1], xz[i * 2], xz[i * 2 + 1];
        Vector3d X = A.colPivHouseholderQr().solve(B);
        poly.vertices[i].x = X(0);
        poly.vertices[i].y = X(1);
        poly.vertices[i].z = X(2);
    }

    // Get faces and edges for the current polyhedron
    for (int i = 0; i < numFaces; i++) {
        int numEdges;
        printf("Enter the number of edges in face %d of the %s polyhedron: ", i + 1, polyType.c_str());
        scanf("%d", &numEdges);
        poly.faces[i].edges.resize(numEdges);

        for (int j = 0; j < numEdges; j++) {
            int v1, v2;
            printf("Enter vertices for edge %d in face %d of the %s polyhedron (format: v1 v2): ", j + 1, i + 1, polyType.c_str());
            scanf("%d %d", &v1, &v2);
            v1--; v2--;  // Convert to 0-based indexing
            
            poly.faces[i].edges[j].v1 = poly.vertices[v1];
            poly.faces[i].edges[j].v2 = poly.vertices[v2];
            
            // Calculate edge length
            double dx = poly.faces[i].edges[j].v2.x - poly.faces[i].edges[j].v1.x;
            double dy = poly.faces[i].edges[j].v2.y - poly.faces[i].edges[j].v1.y;
            double dz = poly.faces[i].edges[j].v2.z - poly.faces[i].edges[j].v1.z;
            poly.faces[i].edges[j].length = sqrt(dx*dx + dy*dy + dz*dz);
        }
    }

    // Ask if there are any sub-polyhedrons (holes) inside this polyhedron
    int numSubPolyhedrons;
    printf("Enter the number of internal hole polyhedrons within the %s polyhedron (0 if none): ", polyType.c_str());
    scanf("%d", &numSubPolyhedrons);
    poly.sub_polyhedrons.resize(numSubPolyhedrons);

    // Recursively get input and perform 3D reconstruction for each sub-polyhedron
    for (int i = 0; i < numSubPolyhedrons; ++i) {
        printf("\nEntering internal hole polyhedron %d of the %s polyhedron\n", i + 1, polyType.c_str());
        getInput(poly.sub_polyhedrons[i], "internal hole " + to_string(i + 1));
    }
}

// Function to print the reconstructed 3D polyhedron, including internal hole polyhedrons
void printPolyhedron(const Polyhedron& poly, const string& polyType, int level) {
    printf("%*sReconstructed 3D %s Polyhedron:\n", level * 2, "", polyType.c_str());

    // Print vertices
    printf("%*sVertices:\n", level * 2, "");
    for (size_t i = 0; i < poly.vertices.size(); ++i) {
        const Vertex& v = poly.vertices[i];
        printf("%*s  Vertex %zu: (%.2f, %.2f, %.2f)\n", level * 2, "", i + 1, v.x, v.y, v.z);
    }

    // Print faces and edges
    for (size_t i = 0; i < poly.faces.size(); i++) {
        printf("%*sFace %zu:\n", level * 2, "", i + 1);
        for (size_t j = 0; j < poly.faces[i].edges.size(); j++) {
            const Edge& edge = poly.faces[i].edges[j];
            printf("%*s  Edge %zu: (%.2f, %.2f, %.2f) to (%.2f, %.2f, %.2f), Length: %.2f\n",
                   level * 2, "", j + 1, edge.v1.x, edge.v1.y, edge.v1.z, edge.v2.x, edge.v2.y, edge.v2.z, edge.length);
        }
    }

    // Recursively print internal hole polyhedrons
    for (size_t i = 0; i < poly.sub_polyhedrons.size(); ++i) {
        printf("\n%*sInternal Hole Polyhedron %zu:\n", level * 2, "", i + 1);
        printPolyhedron(poly.sub_polyhedrons[i], "internal hole " + to_string(i + 1), level + 1);
    }
}