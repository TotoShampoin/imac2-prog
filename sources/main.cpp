#include "Programmation/Space.hpp"
#include <TotoGL.hpp>

int main(int, const char *[]) {
    auto window = TotoGL::Window::create(640, 480, "Boids");
    auto renderer = TotoGL::GL3Renderer::create(window);

    Space space(5);

    space.camera()->position() = { 3, 4, 5 };
    space.camera()->lookAt({ 0, 0, 0 });

    window->on(TotoGL::EventName::WINDOW_RESIZE, std::function([&](const TotoGL::WindowResizeEvent *event) {
                   space.camera()->aspect() = event->size.x / event->size.y;
                   space.camera()->updateProjectionMatrix();
               }));

    auto clock = TotoGL::Clock::create();
    while (!window->shouldClose()) {
        space.update(clock->delta_time());
        space.display(renderer);
    }

    return 0;
}
