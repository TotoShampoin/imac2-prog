#include <TotoGL/TotoGL.hpp>

constexpr auto WIDTH = 1024;
constexpr auto HEIGHT = 768;
constexpr auto FOV = glm::radians(70.);
constexpr auto NEAR = .1;
constexpr auto FAR = 100.;

int main(int argc, const char *argv[]) {
    auto window = TotoGL::Window(WIDTH, HEIGHT, "Boids IMAC");
    auto renderer = TotoGL::Renderer();

    auto camera = TotoGL::Camera::Perspective(FOV, (float)WIDTH / HEIGHT, NEAR, FAR);

    while (!window.shouldClose()) {
        window.draw([&]() {

        });
    }

    return 0;
}
