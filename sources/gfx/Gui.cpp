#include "gfx/Gui.hpp"
#include "prog/imgui-impl.hpp"

#include <imgui.h>

void Gui::draw(
    TotoGL::Window& window, BoidContainer& container,
    int& amount, const size_t& spy_index,
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
        _flags.changing_amount |= ImGui::SliderInt("Amount", &amount, 0, 500);
        _flags.resetting |= ImGui::Button("Reset");
        ImGui::Text("Next boids");
        ImGui::SliderFloat("Min velocity", &container.minVelocity(), 0, 10);
        ImGui::SliderFloat("Max velocity", &container.maxVelocity(), 0, 10);
        ImGui::SliderFloat("Avoid force", &container.boidForceParameters().avoid.force, 0, 1);
        ImGui::SliderFloat("Match force", &container.boidForceParameters().match.force, 0, 1);
        ImGui::SliderFloat("Center force", &container.boidForceParameters().center.force, 0, 1);
        // ImGui::SliderFloat("Attract radius", &container.attractRadius(), 0, 1);
        // ImGui::SliderFloat("Expell radius", &container.expellRadius(), 0, 1);
        // ImGui::SliderFloat("Returning radius", &container.returningRadius(), 0, 1);
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(window_width, window_height), ImGuiCond_Always, ImVec2(1, 1));
        ImGui::SetNextWindowSize(ImVec2(304, 0), ImGuiCond_Always);
        ImGui::Begin("Spy");
        _flags.spying_previous |= ImGui::Button("-");
        ImGui::SameLine();
        ImGui::Text("%zu", spy_index);
        ImGui::SameLine();
        _flags.spying_next |= ImGui::Button("+");
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
}

void Gui::updateStates(
    BoidContainer& container,
    int& amount, size_t& spy_index) {
    if (_flags.changing_amount) {
        container.resize(amount);
        _flags.changing_amount = false;
    }
    if (_flags.resetting) {
        container.resetBoids();
        _flags.resetting = false;
    }
    if (_flags.spying_next) {
        spy_index = (spy_index + 1) % container.boids().size();
        _flags.spying_next = false;
    }
    if (_flags.spying_previous) {
        spy_index = (spy_index + container.boids().size() - 1) % container.boids().size();
        _flags.spying_previous = false;
    }
}