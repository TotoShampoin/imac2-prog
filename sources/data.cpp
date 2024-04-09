#include "data.hpp"

#include "prog/BoidContainer.hpp"
#include "prog/imgui-impl.hpp"

#include <TotoGL/TotoGL.hpp>
#include <imgui.h>

Data::Data(TotoGL::Window& window, TotoGL::Renderer& renderer)
    : window(window)
    , boid_renderer(window, renderer) {
    initImGui(window);
    monitor_texture = TotoGL::BufferTextureFactory::create(TotoGL::BufferTexture(256, 192));
    orbit.bindEvents(window, isImGuiFocused);
    window.on(TotoGL::VectorEventName::WINDOW_SIZE, [&](const TotoGL::VectorEvent& event) {
        glViewport(0, 0, event.x, event.y);
        camera.setPersective(FOV, event.x / event.y, NEAR, FAR);
    });
    window.on(TotoGL::InputEventName::KEY, [&](const TotoGL::InputEvent& event) {
        if (event.action == GLFW_PRESS || event.action == GLFW_REPEAT) {
            if (event.button == GLFW_KEY_W)
                player_direction.z = -1;
            if (event.button == GLFW_KEY_S)
                player_direction.z = 1;
            if (event.button == GLFW_KEY_A)
                player_direction.x = -1;
            if (event.button == GLFW_KEY_D)
                player_direction.x = 1;
            if (event.button == GLFW_KEY_SPACE)
                player_direction.y = 1;
            if (event.button == GLFW_KEY_LEFT_SHIFT)
                player_direction.y = -1;
        }
        if (event.action == GLFW_RELEASE) {
            if (event.button == GLFW_KEY_W)
                player_direction.z = 0;
            if (event.button == GLFW_KEY_S)
                player_direction.z = 0;
            if (event.button == GLFW_KEY_A)
                player_direction.x = 0;
            if (event.button == GLFW_KEY_D)
                player_direction.x = 0;
            if (event.button == GLFW_KEY_SPACE)
                player_direction.y = 0;
            if (event.button == GLFW_KEY_LEFT_SHIFT)
                player_direction.y = 0;
        }
    });
}

void Data::update(const TotoGL::Seconds& delta) {
    container.update(delta);
    player.move(player_direction);
    player.update(delta);
    auto& spied_boid = container.boids()[spy_index];
    monitor_camera.position() = spied_boid.position() - glm::normalize(spied_boid.velocity()) * 2.f;
    monitor_camera.lookAt(spied_boid.position());
    orbit.position() = player.position();
    orbit.apply(camera);
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
    auto update = timer.getDeltaTime();
    timers.clear();
    timers.push_back({ "update", update });
}

void Data::draw(const TotoGL::Seconds& delta) {
    monitor_texture->draw([&]() {
        boid_renderer.render(container, player, monitor_camera);
        timers.push_back({ "monitor rendering", timer.getDeltaTime() });
    });

    window.draw([&]() {
        boid_renderer.render(container, player, camera);
        timers.push_back({ "scene rendering", timer.getDeltaTime() });

        renderImGui([&] {
            float w, h;
            ImGui::SetNextWindowPos(ImVec2(0, window.size()[1]), ImGuiCond_Always, ImVec2(0, 1));
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
            w = ImGui::GetWindowWidth();
            h = ImGui::GetWindowHeight();
            ImGui::End();

            ImGui::SetNextWindowPos(ImVec2(window.size()[0], window.size()[1]), ImGuiCond_Always, ImVec2(1, 1));
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
            ImGui::Image((void*)(intptr_t)monitor_texture->texture().id(),
                ImVec2(monitor_texture->texture().width(), monitor_texture->texture().height()),
                ImVec2(0, 1), ImVec2(1, 0));
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
