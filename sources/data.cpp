#include "data.hpp"

#include "prog/BoidContainer.hpp"
#include "prog/imgui-impl.hpp"

#include <TotoGL/TotoGL.hpp>
#include <imgui.h>

template <uint NEGATIVE_KEY, uint POSITIVE_KEY>
float axisKey(const TotoGL::InputEvent& event) {
    static bool positive_key = false;
    static bool negative_key = false;
    if (event.action == GLFW_PRESS || event.action == GLFW_REPEAT) {
        if (event.button == POSITIVE_KEY)
            positive_key = true;
        if (event.button == NEGATIVE_KEY)
            negative_key = true;
    }
    if (event.action == GLFW_RELEASE) {
        if (event.button == POSITIVE_KEY)
            positive_key = false;
        if (event.button == NEGATIVE_KEY)
            negative_key = false;
    }
    return static_cast<float>(positive_key - negative_key);
}

Data::Data(TotoGL::Window& window, TotoGL::Renderer& renderer)
    : window(window)
    , boid_renderer(window, renderer) {
    initImGui(window);
    orbit.bindEvents(window, isImGuiFocused, true);
    window.on(TotoGL::VectorEventName::WINDOW_SIZE, [&](const TotoGL::VectorEvent& event) {
        glViewport(0, 0, static_cast<int>(event.x), static_cast<int>(event.y));
        camera.setPersective(FOV, static_cast<float>(event.x / event.y), NEAR, FAR);
    });
    window.on(TotoGL::InputEventName::KEY, [&](const TotoGL::InputEvent& event) {
        player_direction.x = axisKey<GLFW_KEY_A, GLFW_KEY_D>(event);
        player_direction.y = axisKey<GLFW_KEY_LEFT_SHIFT, GLFW_KEY_SPACE>(event);
        player_direction.z = axisKey<GLFW_KEY_W, GLFW_KEY_S>(event);
    });
}

void Data::update(const TotoGL::Seconds& delta) {
    // inputs
    if (changing_amount) {
        container.resize(amount);
        changing_amount = false;
    }
    if (resetting) {
        container.resetBoids();
        resetting = false;
    }
    if (spying_next) {
        spy_index = (spy_index + 1) % container.boids().size();
        spying_next = false;
    }
    if (spying_previous) {
        spy_index = (spy_index + container.boids().size() - 1) % container.boids().size();
        spying_previous = false;
    }
    glm::vec3 direction = orbit.front(true) * -player_direction.z + orbit.right(true) * player_direction.x + orbit.up(true) * player_direction.y;

    player.move(direction, delta);

    // update
    container.update(delta);
    const auto& spied_boid = container.boids()[spy_index];
    monitor_camera.position() = spied_boid.position();
    monitor_camera.lookAt(spied_boid.position() + spied_boid.velocity());
    player.update(delta);
    orbit.position() = player.position();
    orbit.apply(camera);

    auto update = timer.getDeltaTime();
    timers.clear();
    timers.emplace_back("update", update);
}

void Data::draw(const TotoGL::Seconds& delta) {
    monitor_texture->draw([&]() {
        boid_renderer.render(container, player, monitor_camera);
        timers.emplace_back("monitor rendering", timer.getDeltaTime());
    });

    window.draw([&]() {
        boid_renderer.render(container, player, camera);
        timers.emplace_back("scene rendering", timer.getDeltaTime());

        renderImGui([&] {
            auto window_size = window.size();
            auto window_width = static_cast<float>(window_size[0]);
            auto window_height = static_cast<float>(window_size[1]);
            auto monitor_width = static_cast<float>(monitor_texture->texture().width());
            auto monitor_height = static_cast<float>(monitor_texture->texture().height());
            auto* monitor_texture_id = reinterpret_cast<ImTextureID>(monitor_texture->texture().id()); // NOLINT

            ImGui::SetNextWindowPos(ImVec2(0, window_height), ImGuiCond_Always, ImVec2(0, 1));
            ImGui::SetNextWindowSize(ImVec2(415, 0), ImGuiCond_Always);
            ImGui::Begin("Controls");
            ImGui::SliderFloat("Avoid", &container.avoidFactor(), 0, 1);
            ImGui::SliderFloat("Match", &container.matchingFactor(), 0, 1);
            ImGui::SliderFloat("Center", &container.centeringFactor(), 0, 1);
            ImGui::SliderFloat("Cube radius", &container.cubeRadius(), 0, 10);
            ImGui::SliderFloat("Attract radius", &container.attractRadius(), 0, 1);
            ImGui::SliderFloat("Expell radius", &container.expellRadius(), 0, 1);
            ImGui::SliderFloat("Returning velocity", &container.returningVelocity(), 1, 20);
            ImGui::SliderFloat("Returning radius", &container.returningRadius(), 0, 1);
            ImGui::SliderFloat("Min velocity", &container.minVelocity(), 0, 10);
            ImGui::SliderFloat("Max velocity", &container.maxVelocity(), 0, 10);
            changing_amount |= ImGui::SliderInt("Amount", &amount, 0, 500);
            resetting |= ImGui::Button("Reset");
            ImGui::End();

            ImGui::SetNextWindowPos(ImVec2(window_width, window_height), ImGuiCond_Always, ImVec2(1, 1));
            ImGui::SetNextWindowSize(ImVec2(304, 0), ImGuiCond_Always);
            ImGui::Begin("Spy");
            spying_previous |= ImGui::Button("-");
            ImGui::SameLine();
            ImGui::Text("%zu", spy_index);
            ImGui::SameLine();
            spying_next |= ImGui::Button("+");
            const auto& boid = container.boids()[spy_index];
            ImGui::Text("Position = %f, %f, %f", boid.position().x, boid.position().y, boid.position().z);
            ImGui::Text("Velocity = %f, %f, %f", boid.velocity().x, boid.velocity().y, boid.velocity().z);
            ImGui::Image(monitor_texture_id, ImVec2(monitor_width, monitor_height), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::End();

            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::Begin("Timers");
            for (const auto& [name, time] : timers) {
                ImGui::Text("%s = %fs", name.c_str(), time);
            }
            ImGui::Text("Frame time = %fs | %3.0fHz", delta, 1 / delta);
            ImGui::End();
        });
    });
}
