#include "data.hpp"

int main(int argc, const char* argv[]) {
    auto window = TotoGL::Window(WIDTH, HEIGHT, "Boids IMAC");
    auto renderer = TotoGL::Renderer();
    auto data = Data(window, renderer);

    while (!window.shouldClose()) {
        data.update(data.clock.getDeltaTime());
        data.draw();
    }

    return 0;
}
