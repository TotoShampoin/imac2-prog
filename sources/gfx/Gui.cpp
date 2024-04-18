#include "gfx/Gui.hpp"
#include "math/variables.hpp"
#include "prog/BoidContainer.hpp"
#include "prog/imgui-impl.hpp"

#include <algorithm>
#include <format>
#include <imgui.h>

void UiRenderer::draw(
    TotoGL::Window& window, BoidContainer& container,
    UiVariables& ui_variables, BoidSpawner& spawner,
    const TotoGL::Seconds& delta,
    const TotoGL::BufferTextureInstanceId& monitor_texture) {

    auto window_size = window.size();
    auto window_width = static_cast<float>(window_size[0]);
    auto window_height = static_cast<float>(window_size[1]);

    renderImGui([&] {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always, ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
        drawSpawnControls(ui_variables, spawner);
        ImGui::SetNextWindowPos(ImVec2(0, window_height), ImGuiCond_Always, ImVec2(0, 1));
        ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
        drawGeneralControls(ui_variables, container);
        ImGui::SetNextWindowPos(ImVec2(window_width, 0), ImGuiCond_Always, ImVec2(1, 0));
        ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
        drawSpyControls(ui_variables, container, monitor_texture);
        ImGui::SetNextWindowPos(ImVec2(window_width, window_height), ImGuiCond_Always, ImVec2(1, 1));
        ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
        drawStatistics(ui_variables, container, spawner, delta);
    });
}

void UiRenderer::drawSpawnControls(UiVariables& ui_variables, BoidSpawner& spawner) {
    ImGui::Begin("Spawn controls");
    ImGui::SliderFloat("Position spread", &spawner.positionRadius(), 0, 1);
    ImGui::SliderFloat("Direction spread", &spawner.boidSpeed(), 0, 3);
    if (ImGui::CollapsingHeader("Forces")) {
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
    }
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
}

void UiRenderer::drawGeneralControls(UiVariables&, BoidContainer& container) {
    ImGui::Begin("General controls");
    ImGui::SliderFloat("Box radius", &container.cubeRadius(), 0, 10);
    ImGui::SliderFloat("Cube force", &container.cubeForce().force, 0, 20);
    ImGui::SliderFloat("Min velocity", &container.minVelocity(), 0, 10);
    ImGui::SliderFloat("Max velocity", &container.maxVelocity(), 0, 10);
    ImGui::End();
}

void UiRenderer::drawSpyControls(UiVariables& ui_variables, BoidContainer& container, const TotoGL::BufferTextureInstanceId& monitor_texture) {
    auto monitor_width = static_cast<float>(monitor_texture->texture().width());
    auto monitor_height = static_cast<float>(monitor_texture->texture().height());
    auto* monitor_texture_id = reinterpret_cast<ImTextureID>(monitor_texture->texture().id()); // NOLINT
    ImGui::Begin("Spy");
    if (container.boids().empty()) {
        ImGui::Text("No boids to spy on");
    } else {
        int current_index = ui_variables.spy_index;
        _flags.changing_spy |= ImGui::InputInt("##spy_index", &ui_variables.spy_index, 1, 10);
        auto& boid = container.boids()[current_index];
        ImGui::Text("Position = %f, %f, %f", boid.position().x, boid.position().y, boid.position().z);
        ImGui::Text("Velocity = %f", glm::length(boid.velocity()));
        ImGui::ColorEdit3("Color", &boid.color().r);
        if (ImGui::CollapsingHeader("Forces")) {
            ImGui::SliderFloat("Avoid force", &boid.avoidForce().force, 0, 1);
            ImGui::SliderFloat("Avoid radius", &boid.avoidForce().zone_width, 0, 2);
            ImGui::SliderFloat("Avoid offset", &boid.avoidForce().zone_offset, 0, 2);
            ImGui::SliderFloat("Match force", &boid.matchForce().force, 0, 1);
            ImGui::SliderFloat("Match radius", &boid.matchForce().zone_width, 0, 2);
            ImGui::SliderFloat("Match offset", &boid.matchForce().zone_offset, 0, 2);
            ImGui::SliderFloat("Center force", &boid.centerForce().force, 0, 1);
            ImGui::SliderFloat("Center radius", &boid.centerForce().zone_width, 0, 2);
            ImGui::SliderFloat("Center offset", &boid.centerForce().zone_offset, 0, 2);
            ImGui::SliderFloat("Bias force", &boid.biasForce().force, 0, 1);
            ImGui::SliderFloat("Bias radius", &boid.biasForce().zone_width, 0, 2);
            ImGui::SliderFloat("Bias offset", &boid.biasForce().zone_offset, 0, 2);
        }
        if (ImGui::CollapsingHeader("Influences")) {
            ImGui::Text("Separation = %f", glm::length(boid.separation(container.boids(), boid.avoidForce())));
            ImGui::Text("Alignment = %f", glm::length(boid.alignment(container.boids(), boid.matchForce())));
            ImGui::Text("Cohesion = %f", glm::length(boid.cohesion(container.boids(), boid.centerForce())));
            ImGui::Text("Bias = %f", glm::length(boid.bias(container.baits(), boid.biasForce())));
        }
        if (ImGui::CollapsingHeader("Monitor")) {
            ImGui::Image(monitor_texture_id, ImVec2(monitor_width, monitor_height), ImVec2(0, 1), ImVec2(1, 0));
        }
    }
    ImGui::End();
}

// TODO(Stats) Get the data from the spawner, not from the simulation itself
void UiRenderer::drawStatistics(UiVariables& ui_variables, BoidContainer& container, BoidSpawner& spawner, const TotoGL::Seconds& delta) {
    ImGui::Begin("Statistics");
    ImGui::Text("Framerate: %3.3f", 1.0 / delta);
    ImGui::Text("Boids: %zu", container.boids().size());
    // tabs
    if (ImGui::BeginTabBar("Statistics")) {
        if (ImGui::BeginTabItem("Boid forces")) {
            drawStatisticsBoidForces(ui_variables, container, spawner);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Boid colors")) {
            drawStatisticsBoidColors(ui_variables, container);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Planet spawning")) {
            // ImGui::Text("Not implemented yet");
            ImGui::InputInt("Amount", &ui_variables.planet_amount);
            _flags.respawn_planets |= ImGui::Button("Regenerate planets");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void UiRenderer::drawStatisticsBoidForces(UiVariables&, BoidContainer& container, BoidSpawner& spawner) {
    constexpr auto HISTOGRAM_SIZE = 25;
    enum WhichForce {
        AVOID,
        MATCH,
        CENTER,
        BIAS,
    };
    static WhichForce which_force = AVOID;
    static std::array<std::string, 4> force_names = { "Avoid", "Match", "Center", "Bias" };
    static auto get_boid_force = [](const Boid& boid) {
        switch (which_force) {
        case AVOID:
            return boid.avoidForce().force;
        case MATCH:
            return boid.matchForce().force;
        case CENTER:
            return boid.centerForce().force;
        case BIAS:
            return boid.biasForce().force;
        }
        return 0.f;
    };
    static auto get_spawner_force = [&spawner] {
        switch (which_force) {
        case AVOID:
            return spawner.boidForceParameters().avoid.force;
        case MATCH:
            return spawner.boidForceParameters().match.force;
        case CENTER:
            return spawner.boidForceParameters().center.force;
        case BIAS:
            return spawner.boidForceParameters().bias.force;
        }
        return 0.f;
    };
    static std::vector<float> histogram(HISTOGRAM_SIZE, 0);
    static std::vector<float> expected_histogram(HISTOGRAM_SIZE, 0);
    float max_count = 0;
    float min_force = 0;
    float max_force = 0;
    auto& _strength_generator = Variables::instance()._boid_strength_generator;
    if (!container.boids().empty()) {
        max_count = 0;
        min_force = std::numeric_limits<float>::max();
        max_force = std::numeric_limits<float>::min();
        for (const auto& boid : container.boids()) {
            auto force = get_boid_force(boid);
            min_force = std::min(min_force, force);
            max_force = std::max(max_force, force);
        }
        std::fill(histogram.begin(), histogram.end(), 0);
        for (const auto& boid : container.boids()) {
            auto force = get_boid_force(boid);
            auto index = static_cast<size_t>((force - min_force) / (max_force - min_force) * (HISTOGRAM_SIZE - 1));
            histogram[index] += 1;
            max_count = std::max(max_count, histogram[index]);
        }
        for (size_t i = 0; i < HISTOGRAM_SIZE; i++) {
            float x = static_cast<float>(i) / (HISTOGRAM_SIZE - 1) * (max_force - min_force) + min_force;
            expected_histogram[i] = _strength_generator.probability(x - get_spawner_force()) * container.boids().size() * (max_force - min_force) / HISTOGRAM_SIZE;
        }
    }

    if (ImGui::BeginCombo("Force type", force_names[which_force].c_str())) {
        if (ImGui::Selectable("Avoid", which_force == AVOID)) {
            which_force = AVOID;
        }
        if (ImGui::Selectable("Match", which_force == MATCH)) {
            which_force = MATCH;
        }
        if (ImGui::Selectable("Center", which_force == CENTER)) {
            which_force = CENTER;
        }
        if (ImGui::Selectable("Bias", which_force == BIAS)) {
            which_force = BIAS;
        }
        ImGui::EndCombo();
    }
    ImGui::PlotHistogram("##forces", histogram.data(), HISTOGRAM_SIZE, 0, nullptr, 0, max_count, ImVec2(0, 100));
    ImGui::PlotLines("##expected", expected_histogram.data(), HISTOGRAM_SIZE, 0, nullptr, 0, max_count, ImVec2(0, 100));
    ImGui::Text("%f - %f", min_force, max_force);
    ImGui::Text("Mean: %f", _strength_generator.mean());
    ImGui::Text("Standard deviation: %f", _strength_generator.standardDeviation());
}

void UiRenderer::drawStatisticsBoidColors(UiVariables&, BoidContainer& container) {
    auto colors = Variables::instance()._boid_color_generator.values();
    for (auto& [color, _] : colors) {
        size_t count = std::count_if(container.boids().begin(), container.boids().end(), [&](const auto& boid) {
            return boid.color() == color;
        });
        std::string color_code = std::format("#{:02X}{:02X}{:02X}", static_cast<int>(255 * color.r), static_cast<int>(255 * color.g), static_cast<int>(255 * color.b));
        ImGui::SliderInt(color_code.c_str(), reinterpret_cast<int*>(&count), 0, static_cast<int>(container.boids().size()), "%d", ImGuiSliderFlags_NoInput);
    }
}

void UiRenderer::updateStates(UiVariables& ui_variables, BoidContainer& container, BoidSpawner& spawner, BoidRenderer& boid_renderer) {
    auto boid_spawner = [&spawner](Boid& boid) { spawner.spawnBoid(boid); };
    auto bait_spawner = [&spawner](Bait& bait) { spawner.spawnBait(bait); };

    if (ui_variables.add_amount < 0) {
        ui_variables.add_amount = 0;
    }
    if (_flags.destroy_boids) {
        container.destroyBoids(container.boids().size());
        _flags.destroy_boids = false;
    }
    if (_flags.changing_spy) {
        if (ui_variables.spy_index < 0) {
            ui_variables.spy_index = 0;
        }
        if (ui_variables.spy_index >= static_cast<int>(container.boids().size())) {
            ui_variables.spy_index = static_cast<int>(container.boids().size()) - 1;
        }
        _flags.changing_spy = false;
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
    if (_flags.respawn_planets) {
        boid_renderer.regeneratePlanets(ui_variables.planet_amount);
        _flags.respawn_planets = false;
    }

    ui_variables.amount = static_cast<int>(container.boids().size());
}
