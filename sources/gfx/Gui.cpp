#include "gfx/Gui.hpp"
#include "math/variables.hpp"
#include "prog/imgui-impl.hpp"

#include <format>
#include <imgui.h>

void UiRenderer::draw(
    TotoGL::Window& window, BoidContainer& container,
    UiVariables& ui_variables, BoidSpawner& spawner,
    const TotoGL::Seconds& delta,
    const std::vector<std::pair<std::string, float>>&,
    const TotoGL::BufferTextureInstanceId& monitor_texture) {

    renderImGui([&] {
        auto window_size = window.size();
        auto window_width = static_cast<float>(window_size[0]);
        auto window_height = static_cast<float>(window_size[1]);
        auto monitor_width = static_cast<float>(monitor_texture->texture().width());
        auto monitor_height = static_cast<float>(monitor_texture->texture().height());
        auto* monitor_texture_id = reinterpret_cast<ImTextureID>(monitor_texture->texture().id()); // NOLINT

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always, ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
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
        ImGui::InputInt("##add_amount", &ui_variables.add_amount, 10, 50);
        _flags.add_boid |= ImGui::Button("Add boid");
        ImGui::SameLine();
        _flags.remove_boid |= ImGui::Button("Remove boid");
        ImGui::SameLine();
        _flags.destroy_boids |= ImGui::Button("Destroy all boids");
        _flags.add_bait |= ImGui::Button("Add a bait");
        ImGui::SameLine();
        _flags.destroy_baits |= ImGui::Button("Destroy all baits");
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(0, window_height), ImGuiCond_Always, ImVec2(0, 1));
        ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::Begin("General controls");
        ImGui::SliderFloat("Box radius", &container.cubeRadius(), 0, 10);
        ImGui::SliderFloat("Cube force", &container.cubeForce().force, 0, 20);
        ImGui::SliderFloat("Min velocity", &container.minVelocity(), 0, 10);
        ImGui::SliderFloat("Max velocity", &container.maxVelocity(), 0, 10);
        // _flags.changing_amount |= ImGui::SliderInt("Amount", &ui_variables.amount, 0, 500);
        // _flags.resetting |= ImGui::Button("Reset");
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(window_width, 0), ImGuiCond_Always, ImVec2(1, 0));
        ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::Begin("Spy");
        if (container.boids().empty()) {
            ImGui::Text("No boids to spy on");
        } else {
            _flags.spying_previous |= ImGui::Button("-");
            ImGui::SameLine();
            ImGui::Text("%zu", ui_variables.spy_index);
            ImGui::SameLine();
            _flags.spying_next |= ImGui::Button("+");
            // ImGui::InputInt("Spy index", &ui_variables.spy_index, 1, 1);
            const auto& boid = container.boids()[ui_variables.spy_index];
            ImGui::Text("Position = %f, %f, %f", boid.position().x, boid.position().y, boid.position().z);
            ImGui::Text("Velocity = %f, %f, %f", boid.velocity().x, boid.velocity().y, boid.velocity().z);
            ImGui::Image(monitor_texture_id, ImVec2(monitor_width, monitor_height), ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(window_width, window_height), ImGuiCond_Always, ImVec2(1, 1));
        ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::Begin("Statistics");
        ImGui::Text("Framerate: %f", 1.0 / delta);
        // tabs
        if (ImGui::BeginTabBar("Statistics")) {
            if (ImGui::BeginTabItem("Boid colors")) {
                auto colors = Variables::instance()._boid_color_generator.values();
                for (auto& [color, _] : colors) {
                    size_t count = std::count_if(container.boids().begin(), container.boids().end(), [&](const auto& boid) {
                        return boid.color() == color;
                    });
                    std::string color_code = std::format("#{:02X}{:02X}{:02X}", static_cast<int>(255 * color.r), static_cast<int>(255 * color.g), static_cast<int>(255 * color.b));
                    ImGui::SliderInt(color_code.c_str(), reinterpret_cast<int*>(&count), 0, static_cast<int>(container.boids().size()), "%d", ImGuiSliderFlags_NoInput);
                }
                ImGui::EndTabItem();
            }
            // if (ImGui::BeginTabItem("Boid forces")) {
            //     constexpr auto HISTOGRAM_SIZE = 10;
            //     std::vector<float> avoid_forces;
            //     for (const auto& boid : container.boids()) {
            //         avoid_forces.push_back(boid.avoidForce().force);
            //     }
            //     if (!avoid_forces.empty()) {
            //         std::sort(avoid_forces.begin(), avoid_forces.end());
            //         auto avoid_min = avoid_forces.front();
            //         auto avoid_max = avoid_forces.back();
            //         auto avoid_range = avoid_max - avoid_min;
            //         std::vector<float> histogram(HISTOGRAM_SIZE, 0);
            //         for (const auto& force : avoid_forces) {
            //             auto index = static_cast<size_t>((force - avoid_min) / avoid_range * HISTOGRAM_SIZE);
            //             histogram[index] += 1;
            //         }
            //         auto max_count = *std::max_element(histogram.begin(), histogram.end());
            //         ImGui::PlotHistogram("Avoid forces", histogram.data(), HISTOGRAM_SIZE, 0, nullptr, 0, max_count, ImVec2(0, 100));
            //     } else {
            //         ImGui::PlotHistogram("Avoid forces", nullptr, 0, 0, nullptr, 0, 0, ImVec2(0, 100));
            //     }
            //     ImGui::EndTabItem();
            // }
            // ImGui::EndTabBar();
        }
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
    if (_flags.destroy_boids) {
        container.destroyBoids(container.boids().size());
        _flags.destroy_boids = false;
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
