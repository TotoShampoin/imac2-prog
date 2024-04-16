#include "gfx/Gui.hpp"
#include "prog/imgui-impl.hpp"

#include <imgui.h>

void UiRenderer::draw(
    TotoGL::Window& window, BoidContainer& container,
    UiVariables& ui_variables, BoidSpawner& spawner,
    const TotoGL::Seconds& delta,
    const std::vector<std::pair<std::string, float>>& timers,
    const TotoGL::BufferTextureInstanceId& monitor_texture) {
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
        ImGui::Text("General");
        ImGui::SliderFloat("Box radius", &container.cubeRadius(), 0, 10);
        ImGui::SliderFloat("Cube force", &container.cubeForce().force, 0, 20);
        ImGui::SliderFloat("Min velocity", &container.minVelocity(), 0, 10);
        ImGui::SliderFloat("Max velocity", &container.maxVelocity(), 0, 10);
        _flags.changing_amount |= ImGui::SliderInt("Amount", &ui_variables.amount, 0, 500);
        _flags.resetting |= ImGui::Button("Reset");
        ImGui::Text("Next boids");
        ImGui::SliderFloat("Avoid force", &spawner.boidForceParameters().avoid.force, 0, 1);
        ImGui::SliderFloat("Match force", &spawner.boidForceParameters().match.force, 0, 1);
        ImGui::SliderFloat("Center force", &spawner.boidForceParameters().center.force, 0, 1);
        ImGui::SliderFloat("Position radius", &spawner.positionRadius(), 0, 1);
        ImGui::SliderFloat("Speed", &spawner.boidSpeed(), 0, 1);
        // ImGui::SliderFloat("Attract radius", &container.attractRadius(), 0, 1);
        // ImGui::SliderFloat("Expell radius", &container.expellRadius(), 0, 1);
        // ImGui::SliderFloat("Returning radius", &container.returningRadius(), 0, 1);
        ImGui::SliderInt("##", &ui_variables.add_amount, 0, 50);
        ImGui::SameLine();
        _flags.add_boid |= ImGui::Button("Add boid");
        ImGui::SameLine();
        _flags.remove_boid |= ImGui::Button("Remove boid");
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(window_width, window_height), ImGuiCond_Always, ImVec2(1, 1));
        ImGui::SetNextWindowSize(ImVec2(304, 0), ImGuiCond_Always);
        ImGui::Begin("Spy");
        if (container.boids().empty()) {
            ImGui::Text("No boids to spy on");
        } else {
            _flags.spying_previous |= ImGui::Button("-");
            ImGui::SameLine();
            ImGui::Text("%zu", ui_variables.spy_index);
            ImGui::SameLine();
            _flags.spying_next |= ImGui::Button("+");
            const auto& boid = container.boids()[ui_variables.spy_index];
            ImGui::Text("Position = %f, %f, %f", boid.position().x, boid.position().y, boid.position().z);
            ImGui::Text("Velocity = %f, %f, %f", boid.velocity().x, boid.velocity().y, boid.velocity().z);
            ImGui::Image(monitor_texture_id, ImVec2(monitor_width, monitor_height), ImVec2(0, 1), ImVec2(1, 0));
        }
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
}

void UiRenderer::updateStates(
    BoidContainer& container,
    UiVariables& ui_variables,
    const std::function<void(Boid&)>& spawner) {
    if (_flags.changing_amount) {
        container.resize(ui_variables.amount, spawner);
        _flags.changing_amount = false;
    }
    if (_flags.resetting) {
        container.resetBoids(container.boids().size(), spawner);
        _flags.resetting = false;
    }
    if (_flags.spying_next) {
        ui_variables.spy_index = (ui_variables.spy_index + 1) % container.boids().size();
        _flags.spying_next = false;
    }
    if (_flags.spying_previous) {
        ui_variables.spy_index = (ui_variables.spy_index + container.boids().size() - 1) % container.boids().size();
        _flags.spying_previous = false;
    }
    if (_flags.add_boid) {
        container.addBoids(ui_variables.add_amount, spawner);
        ui_variables.amount = static_cast<int>(container.boids().size());
        _flags.add_boid = false;
    }
    if (_flags.remove_boid) {
        // unsigned integers can be a pain in the ass sometimes
        if (static_cast<int>(container.boids().size()) - ui_variables.add_amount < 0) {
            ui_variables.add_amount = 0;
        }
        container.destroyBoids(ui_variables.add_amount);
        ui_variables.amount = static_cast<int>(container.boids().size());
        _flags.remove_boid = false;
    }
}
