#include "projections.h"

using namespace std;
using namespace Eigen;


void orthographicProjectionCustomPlane(const Polyhedron& poly, float A, float B, float C, float D) {
    (void)D;
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Orthographic Projection on Custom Plane", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Helper function to project and render a polyhedron
    auto renderPolyhedron = [&](const Polyhedron& polyToRender) {
        for (const auto& face : polyToRender.faces) {
            for (const auto& edge : face.edges) {
                // Determine the color based on the edge's orientation
                Vector3d v1(edge.v1.x, edge.v1.y, edge.v1.z);
                Vector3d v2(edge.v2.x, edge.v2.y, edge.v2.z);
                Vector3d edgeDirection = v2 - v1;

                // Normal vector of the plane
                Vector3d planeNormal(A, B, C);

                // Calculate the dot product to determine facing direction
                if (edgeDirection.dot(planeNormal) > 0) {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Front-facing edges are black
                } else {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Back-facing edges are red
                }

                // Project the edge onto the plane
                int x1 = static_cast<int>(edge.v1.x * 100 + WINDOW_WIDTH / 2);
                int y1 = static_cast<int>(edge.v1.y * 100 + WINDOW_HEIGHT / 2);
                int x2 = static_cast<int>(edge.v2.x * 100 + WINDOW_WIDTH / 2);
                int y2 = static_cast<int>(edge.v2.y * 100 + WINDOW_HEIGHT / 2);

                SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
            }
        }
    };

    // Render the outer polyhedron first
    renderPolyhedron(poly);

    // Render each sub-polyhedron (hole) within the same window
    for (const auto& subPoly : poly.sub_polyhedrons) {
        renderPolyhedron(subPoly);
    }

    SDL_RenderPresent(renderer);  // Present the rendered image to the screen

    // Wait for a quit event to close the window
    SDL_Event event;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Project a 3D vertex to 2D
SDL_Point projectTo2D(Vertex v, float angleX, float angleY) {
    // Rotate around X axis
    float cosX = cos(angleX), sinX = sin(angleX);
    float yRot = v.y * cosX - v.z * sinX;
    float zRot = v.y * sinX + v.z * cosX;

    // Rotate around Y axis
    float cosY = cos(angleY), sinY = sin(angleY);
    float xRot = v.x * cosY + zRot * sinY;
    float zFinal = -v.x * sinY + zRot * cosY;

    // Apply the scale factor
    xRot *= SCALE_FACTOR;
    yRot *= SCALE_FACTOR;
    zFinal *= SCALE_FACTOR;

    // Isometric projection onto 2D
    int x2D = static_cast<int>((xRot - yRot) * cos(ISO_ANGLE) + SCREEN_WIDTH / 2);
    int y2D = static_cast<int>((xRot + yRot) * sin(ISO_ANGLE) - zFinal + SCREEN_HEIGHT / 2);

    return {x2D, y2D};
}

// Draw a polyhedron with a specified color, including its sub-polyhedrons
void drawPolyhedron(SDL_Renderer* renderer, const Polyhedron& polyhedron, float angleX, float angleY, SDL_Color color) {
    // Set color for the current polyhedron
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Draw edges for each face
    for (const auto& face : polyhedron.faces) {
        for (const auto& edge : face.edges) {
            SDL_Point p1 = projectTo2D(edge.v1, angleX, angleY);
            SDL_Point p2 = projectTo2D(edge.v2, angleX, angleY);
            SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
        }
    }

    // Draw each sub-polyhedron in a different color
    SDL_Color subColor = {
        static_cast<Uint8>(255 - color.r),
        static_cast<Uint8>(255 - color.g),
        static_cast<Uint8>(255 - color.b),
        255 // Full opacity
    };
    for (const auto& sub : polyhedron.sub_polyhedrons) {
        drawPolyhedron(renderer, sub, angleX, angleY, subColor);
    }
}