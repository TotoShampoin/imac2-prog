#include "simulation.hpp"

int main(int /* argc */, const char* /* argv */[]) {
    auto window = TotoGL::Window(WIDTH, HEIGHT, "Boids IMAC", false);
    auto renderer = TotoGL::Renderer();
    auto simulation = Simulation(window, renderer);

    while (!window.shouldClose()) {
        auto delta = simulation.getDeltaTime();
        simulation.update(delta);
        simulation.draw(delta);
    }

    return 0;
}
