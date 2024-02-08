#include "prog/BoidSpace.hpp"
#include <TotoGL/TotoGL.hpp>
#include <fstream>

#include <imgui.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

constexpr auto WIDTH = 1024;
constexpr auto HEIGHT = 768;
constexpr auto FOV = glm::radians(70.);
constexpr auto NEAR = .1;
constexpr auto FAR = 100.;

using TotoGL::VectorEventName::WINDOW_SIZE;

void ui_init(TotoGL::Window&);
void ui_show(BoidSpace&);

int main(int argc, const char* argv[]) {
    auto window = TotoGL::Window(WIDTH, HEIGHT, "Boids IMAC");
    auto renderer = TotoGL::Renderer();

    const auto object_instance = TotoGL::RenderObjectFactory::create(TotoGL::RenderObject(
        TotoGL::MeshFactory::create(
            TotoGL::Mesh::cube()),
        TotoGL::ShaderMaterialFactory::create(TotoGL::ShaderMaterial(
            TotoGL::VertexShader(std::ifstream("assets/shaders/shader.vert")),
            TotoGL::FragmentShader(std::ifstream("assets/shaders/shader.frag"))))));

    auto& object = TotoGL::RenderObjectFactory::get(object_instance);
    object.material().uniform("u_color", glm::vec4(1, 1, 1, 1));

    auto camera = TotoGL::Camera::Perspective(FOV, (float)WIDTH / HEIGHT, NEAR, FAR);
    camera.position() = { 2, 2, 2 };
    camera.lookAt({ 0, 0, 0 });

    auto orbit = TotoGL::OrbitControl(0, 0, 3);
    orbit.bindEvents(window);

    auto space = BoidSpace(48, object_instance);

    auto clock = TotoGL::Clock();

    window.on(WINDOW_SIZE, [&](const TotoGL::VectorEvent& event) {
        glViewport(0, 0, event.x, event.y);
        camera.setPersective(FOV, event.x / event.y, NEAR, FAR);
    });

    ui_init(window);
    while (!window.shouldClose()) {
        auto delta = clock.getDeltaTime();
        orbit.apply(camera);
        space.update(delta);

        window.draw([&]() {
            renderer.clear();

            space.render(renderer, camera);

            ui_show(space);
        });
    }

    return 0;
}

void ui_init(TotoGL::Window& window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui_ImplGlfw_InitForOpenGL(window.glfwWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 460");

    auto& style = ImGui::GetStyle();
    style.FrameRounding = 2.;
    style.WindowRounding = 4.;
}

void ui_show(BoidSpace& space) {
    glm::vec3 avg_velocity = glm::vec3(0);
    for (const auto& boid : space.boids()) {
        avg_velocity += boid.velocity();
    }
    avg_velocity /= space.boids().size();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Camera");
    ImGui::Text("avg velocity = %f, %f, %f", avg_velocity.x, avg_velocity.y, avg_velocity.z);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
