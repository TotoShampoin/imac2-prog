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
        ImGui::Begin("General controls");
        ImGui::SliderFloat("Box radius", &container.cubeRadius(), 0, 10);
        ImGui::SliderFloat("Cube force", &container.cubeForce().force, 0, 20);
        ImGui::SliderFloat("Min velocity", &container.minVelocity(), 0, 10);
        ImGui::SliderFloat("Max velocity", &container.maxVelocity(), 0, 10);
        _flags.changing_amount |= ImGui::SliderInt("Amount", &ui_variables.amount, 0, 500);
        _flags.resetting |= ImGui::Button("Reset");
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(window_width, 0), ImGuiCond_Always, ImVec2(1, 0));
        ImGui::SetNextWindowSize(ImVec2(415, 0), ImGuiCond_Always);
        ImGui::Begin("Spawn controls");
        ImGui::SliderFloat("Spawn spread", &spawner.positionRadius(), 0, 1);
        ImGui::SliderFloat("Speed", &spawner.boidSpeed(), 0, 3);
        ImGui::SliderFloat("Avoid force", &spawner.boidForceParameters().avoid.force, 0, 1);
        ImGui::SliderFloat("Avoid radius", &spawner.boidForceParameters().avoid.zone_width, 0, 2);
        ImGui::SliderFloat("Avoid offset", &spawner.boidForceParameters().avoid.zone_offset, 0, 2);
        ImGui::SliderFloat("Match force", &spawner.boidForceParameters().match.force, 0, 1);
        ImGui::SliderFloat("Match radius", &spawner.boidForceParameters().match.zone_width, 0, 2);
        ImGui::SliderFloat("Match offset", &spawner.boidForceParameters().match.zone_offset, 0, 2);
        ImGui::SliderFloat("Center force", &spawner.boidForceParameters().center.force, 0, 1);
        ImGui::SliderFloat("Center radius", &spawner.boidForceParameters().center.zone_width, 0, 2);
        ImGui::SliderFloat("Center offset", &spawner.boidForceParameters().center.zone_offset, 0, 2);
        ImGui::SliderFloat("Bias force", &spawner.boidForceParameters().bias.force, 0, 1);
        ImGui::SliderFloat("Bias radius", &spawner.boidForceParameters().bias.zone_width, 0, 2);
        ImGui::SliderFloat("Bias offset", &spawner.boidForceParameters().bias.zone_offset, 0, 2);

        ImGui::InputInt("##add_amount", &ui_variables.add_amount, 1, 20);
        ImGui::SameLine();
        _flags.add_boid |= ImGui::Button("Add boid");
        ImGui::SameLine();
        _flags.remove_boid |= ImGui::Button("Remove boid");

        _flags.add_bait |= ImGui::Button("Add bait");
        ImGui::SameLine();
        _flags.destroy_baits |= ImGui::Button("Destroy baits");

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

        ImGui::SetNextWindowPos(ImVec2(1, 0), ImGuiCond_Always);
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
    BoidSpawner& spawner) {
    auto boid_spawner = [&spawner](Boid& boid) { spawner.spawnBoid(boid); };
    auto bait_spawner = [&spawner](Bait& bait) { spawner.spawnBait(bait); };

    if (ui_variables.add_amount < 0) {
        ui_variables.add_amount = 0;
    }
    if (_flags.changing_amount) {
        container.resize(ui_variables.amount, boid_spawner);
        _flags.changing_amount = false;
    }
    if (_flags.resetting) {
        container.resetBoids(container.boids().size(), boid_spawner);
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
        container.addBoids(ui_variables.add_amount, boid_spawner);
        _flags.add_boid = false;
    }
    if (_flags.remove_boid) {
        container.destroyBoids(ui_variables.add_amount);
        _flags.remove_boid = false;
    }
    if (_flags.add_bait) {
        Bait new_bait {};
        spawner.spawnBait(new_bait);
        container.addBait(new_bait);
        _flags.add_bait = false;
    }
    if (_flags.destroy_baits) {
        container.destroyBaits();
        _flags.destroy_baits = false;
    }

    ui_variables.amount = static_cast<int>(container.boids().size());
}
