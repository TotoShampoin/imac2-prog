#include "data.hpp"

int main(int /* argc */, const char* /* argv */[]) {
    auto window = TotoGL::Window(WIDTH, HEIGHT, "Boids IMAC", false);
    auto renderer = TotoGL::Renderer();
    auto data = Data(window, renderer);

    while (!window.shouldClose()) {
        auto delta = data.getDeltaTime();
        data.update(delta);
        data.draw(delta);
    }

    return 0;
}
