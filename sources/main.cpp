#include <iostream>

#include <TotoGL.hpp>

int main(int argc, const char* argv[]) {
    auto window = TotoGL::Window::create(640, 480, "Boids");
    auto renderer = TotoGL::Renderer::create(window);

    while(!window->shouldClose()) {

        window->swapBuffer();
        window->pollEvents();
    }

    return 0;
}
