#include "input.h"
#include "validity.h"
#include "geometry.h"
#include "projections.h"
#include "constants.h"
#include "transformations.h"

using namespace std;
using namespace Eigen;

int main() {
    Polyhedron poly;
    int task;
    Vertex origin = {0, 0, 0};
    double density = 1.0;

    getInput(poly, "outer");

    printPolyhedron(poly, "outer", 1);

    // Validate the input
    bool isValid = validateInput(poly);
    
    if (isValid) {
        cout << "The input and reconstruction are valid.\n";
    } else {
        cout << "The input or reconstruction is invalid.\n";
        return 1;  // Exit if the input is invalid
    }

    // Main loop for performing tasks
    while (true) {
        cout << "\nSelect a task to perform:\n";
        cout << "1. Calculate Surface Area\n";
        cout << "2. Calculate Volume\n";
        cout << "3. Calculate Center of Mass\n";
        cout << "4. Transform Polyhedron\n";
        cout << "6. Isometric View\n";
        cout << "7. Orthographic Projection onto Custom Plane\n";
        cout << "8. Calculate Moment of Inertia\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";
        cin >> task;

        if (task == 9) {
            cout << "Exiting the program.\n";
            break;
        }

        if (task == 4) {  // Transform Polyhedron
            transform_polyhedron(poly);
        }

        if (task == 1) {  // Calculate Surface Area
            float surfaceArea = calculateSurfaceArea(poly);
            cout << "The Surface Area of the polyhedron is: " << surfaceArea << endl;
        }

        if (task == 2) {  // Calculate Volume
            float volume = calculatepolyhedronVolume(poly);
            cout << "The Volume of the polyhedron is: " << volume << endl;
        }
        
        if (task == 3) {  // Calculate Centre of Mass
            Vertex centre = calculateCenterOfMass(poly);
            cout << "The Centre of Mass of the polyhedron is: " << centre.x << ", " << centre.y << ", " << centre.z << endl;
        }

        if (task == 8) {  // Calculate Moment of Inertia

            InertiaTensor inertia = computePolyhedronInertia(poly, origin, density);

            // Output the computed inertia tensor
            printf("Inertia Tensor: \nIxx: %lf, Iyy: %lf, Izz: %lf\n", inertia.Ixx, inertia.Iyy, inertia.Izz);
            printf("Ixy: %lf, Ixz: %lf, Iyz: %lf\n", inertia.Ixy, inertia.Ixz, inertia.Iyz);
        }

        if (task == 6) {

            SDL_Init(SDL_INIT_VIDEO);

            SDL_Window* window = SDL_CreateWindow("Isometric View", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
            SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

            float angleX = 0.5f, angleY = 0.5f;
            bool running = true;

            while (running) {
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        running = false;
                    } else if (event.type == SDL_MOUSEMOTION && (event.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT))) {
                        angleX += event.motion.yrel * 0.01f;
                        angleY += event.motion.xrel * 0.01f;
                    }
                }

                SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255); // Light gray background
                SDL_RenderClear(renderer);

                // Define colors for outer and inner polyhedrons
                SDL_Color outerColor = {0, 100, 255, 255};  // Outer polyhedron color (e.g., blue)
                SDL_Color innerColor = {255, 100, 100, 255}; // Inner polyhedron color (e.g., red)

                // Draw the outer polyhedron
                drawPolyhedron(renderer, poly, angleX, angleY, outerColor);

                // Draw sub-polyhedrons with the distinct inner color
                for (const auto& sub : poly.sub_polyhedrons) {
                    drawPolyhedron(renderer, sub, angleX, angleY, innerColor);
                }

                SDL_RenderPresent(renderer);
                SDL_Delay(16); // Approximately 60 FPS
            }

            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();

        }

        if (task == 7) {
            float A, B, C, D;
            std::cout << "Enter the coefficients of the plane equation (Ax + By + Cz = D):" << std::endl;
            std::cout << "A (normal x-component): ";
            std::cin >> A;
            std::cout << "B (normal y-component): ";
            std::cin >> B;
            std::cout << "C (normal z-component): ";
            std::cin >> C;
            std::cout << "D (distance from origin): ";
            std::cin >> D;
            orthographicProjectionCustomPlane(poly, A, B, C, D);
        }
    }


    return 0;
}
