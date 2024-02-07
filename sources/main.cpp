#include <TotoGL/TotoGL.hpp>
#include <fstream>

constexpr auto WIDTH = 1024;
constexpr auto HEIGHT = 768;
constexpr auto FOV = glm::radians(70.);
constexpr auto NEAR = .1;
constexpr auto FAR = 100.;

int main(int argc, const char* argv[]) {
    auto window = TotoGL::Window(WIDTH, HEIGHT, "Boids IMAC");
    auto renderer = TotoGL::Renderer();

    const auto mesh_instance = TotoGL::MeshFactory::create(
        TotoGL::Mesh::cube());
    const auto material_instance = TotoGL::ShaderMaterialFactory::create(
        TotoGL::ShaderMaterial(
            TotoGL::VertexShader(std::ifstream("assets/shaders/shader.vert")),
            TotoGL::FragmentShader(std::ifstream("assets/shaders/shader.frag"))));

    const auto object_instance = TotoGL::RenderObjectFactory::create(
        TotoGL::RenderObject(mesh_instance, material_instance));

    auto& object = TotoGL::RenderObjectFactory::get(object_instance);

    object.material().uniform("u_color", glm::vec4(1, 0, 0, 1));

    auto camera = TotoGL::Camera::Perspective(FOV, (float)WIDTH / HEIGHT, NEAR, FAR);
    camera.position() = { 2, 2, 2 };
    camera.lookAt({ 0, 0, 0 });

    auto clock = TotoGL::Clock();

    while (!window.shouldClose()) {
        auto delta = clock.getDeltaTime();
        object.rotation() += glm::vec3(1, 1, 1) * delta;
        window.draw([&]() {
            renderer.clear();
            renderer.render(object, camera);
        });
    }

    return 0;
}
