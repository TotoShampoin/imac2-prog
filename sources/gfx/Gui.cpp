#include "gfx/Gui.hpp"
#include "gfx/BoidRenderer.hpp"
#include "math/histogram.hpp"
#include "math/variables.hpp"
#include "prog/BoidContainer.hpp"
#include "prog/imgui-impl.hpp"

#include <algorithm>
#include <array>
#include <format>
#include <functional>
#include <glm/geometric.hpp>
#include <imgui.h>

void UiRenderer::draw(
    UiVariables& ui_variables,
    BoidContainer& container, BoidSpawner& spawner, BoidRenderer& boid_renderer,
    TotoGL::Window& window, const TotoGL::Seconds& delta,
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
        drawStatistics(ui_variables, container, spawner, boid_renderer, delta);
    });
}

void UiRenderer::drawSpawnControls(UiVariables& ui_variables, BoidSpawner& spawner) {
    ImGui::Begin("Spawn controls");
    ImGui::InputInt("##add_amount", &ui_variables.add_amount, 10, 50);
    _flags.add_boid |= ImGui::Button("Add boid");
    ImGui::SameLine();
    _flags.remove_boid |= ImGui::Button("Remove boid");
    ImGui::SameLine();
    _flags.destroy_boids |= ImGui::Button("Destroy all boids");
    _flags.add_bait |= ImGui::Button("Add a bait");
    ImGui::SameLine();
    _flags.destroy_baits |= ImGui::Button("Destroy all baits");
    if (ImGui::CollapsingHeader("Movement")) {
        ImGui::SliderFloat("Position spread", &spawner.positionRadius(), 0, 1);
        ImGui::SliderFloat("Speed min", &spawner.boidSpeedCaps().min, 0, 10);
        ImGui::SliderFloat("Speed max", &spawner.boidSpeedCaps().max, 0, 10);
        ImGui::SliderFloat("Speed tendency", &spawner.boidSpeedTendency(), 0, 1);
    }
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
        ImGui::SliderFloat("Force spread", &spawner.boidForceSpread(), 0, 1);
    }
    if (ImGui::CollapsingHeader("Colors")) {
        auto& colors = Variables::instance()._boid_color_generator;
        int i = 0;
        for (auto& [color, weight] : colors.values()) {
            _flags.color_edit |= ImGui::ColorEdit3(std::format("##color-{}", i).c_str(), &color.r);
            _flags.color_edit |= ImGui::SliderFloat(std::format("##weight-{}", i).c_str(), &weight, 0, 10);
            ImGui::SameLine();
            ImGui::Text("%3.2f%%", weight / colors.total() * 100);
            // ImGui::NewLine();
            i++;
        }
    }
    ImGui::End();
}

void UiRenderer::drawGeneralControls(UiVariables& ui_variables, BoidContainer& container) {
    ImGui::Begin("General controls");
    ImGui::SliderFloat("Box radius", &container.cubeRadius(), 0, 10);
    ImGui::SliderFloat("Cube force", &container.cubeForce().force, 0, 20);
    ImGui::SliderInt("##planet_amount", &ui_variables.planet_amount, 0, 100);
    ImGui::SameLine();
    _flags.respawn_planets |= ImGui::Button("Regenerate planets");
    ImGui::End();
}

void UiRenderer::drawSpyControls(UiVariables& ui_variables, BoidContainer& container, const TotoGL::BufferTextureInstanceId& monitor_texture) {
    auto monitor_width = static_cast<float>(monitor_texture->texture().width());
    auto monitor_height = static_cast<float>(monitor_texture->texture().height());
    auto* monitor_texture_id = reinterpret_cast<ImTextureID>(monitor_texture->texture().id()); // NOLINT
    ImGui::Begin("Boid monitoring");
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

void UiRenderer::drawStatistics(UiVariables& ui_variables, BoidContainer& container, BoidSpawner& spawner, BoidRenderer& boid_renderer, const TotoGL::Seconds& delta) {
    ImGui::Begin("Statistics");
    ImGui::Text("Framerate: %3.3f", 1.0 / delta);
    ImGui::Text("Boids: %zu", container.boids().size());
    // tabs
    if (ImGui::BeginTabBar("Statistics")) {
        if (ImGui::BeginTabItem("Boids spawning")) {
            drawStatisticsBoidSpawning(ui_variables, container, spawner);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Planet spawning")) {
            drawStatisticsPlanetSpawning(ui_variables, boid_renderer);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Box colors")) {
            drawStatisticsBoxColors(ui_variables, boid_renderer);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void UiRenderer::drawStatisticsBoidSpawning(UiVariables& ui_variables, BoidContainer& container, BoidSpawner& spawner) {
    enum WhichBoidStats {
        FORCE,
        SPEED,
        COLOR,
    };
    static WhichBoidStats which_boid_stats = FORCE;
    static std::array<std::string, 3> boid_stats_names = { "Forces", "Speed", "Color" };

    if (ImGui::BeginCombo("##boidattribute", boid_stats_names[which_boid_stats].c_str())) {
        if (ImGui::Selectable("Forces", which_boid_stats == FORCE)) {
            which_boid_stats = FORCE;
        }
        if (ImGui::Selectable("Speed", which_boid_stats == SPEED)) {
            which_boid_stats = SPEED;
        }
        if (ImGui::Selectable("Color", which_boid_stats == COLOR)) {
            which_boid_stats = COLOR;
        }
        ImGui::EndCombo();
    }
    switch (which_boid_stats) {
    case FORCE:
        drawStatisticsBoidForces(ui_variables, container, spawner);
        break;
    case SPEED:
        drawStatisticsBoidSpeed(ui_variables, container, spawner);
        break;
    case COLOR:
        drawStatisticsBoidColors(ui_variables, container);
        break;
    }
}

void UiRenderer::drawStatisticsBoidForces(UiVariables&, BoidContainer&, BoidSpawner& spawner) {
    enum WhichForce {
        AVOID,
        MATCH,
        CENTER,
        BIAS,
    };
    static WhichForce which_force = AVOID;
    auto get_stats_force = [&spawner] {
        switch (which_force) {
        case AVOID:
            return spawner.stats().forces.avoid;
        case MATCH:
            return spawner.stats().forces.match;
        case CENTER:
            return spawner.stats().forces.center;
        case BIAS:
            return spawner.stats().forces.bias;
        }
        return std::vector<float>();
    };
    auto get_spawner_force = [&spawner] {
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

    constexpr auto HISTOGRAM_SIZE = 15;
    auto boid_histogram = ProbabilityHistogram<float>(HISTOGRAM_SIZE, get_stats_force(), [&](const float& force) { return force; });
    auto expected_histogram = std::vector<float>(HISTOGRAM_SIZE, 0);
    auto& _strength_generator = Variables::instance()._boid_strength_generator;
    for (size_t i = 0; i < HISTOGRAM_SIZE; i++) {
        float x = static_cast<float>(i) / (HISTOGRAM_SIZE - 1) * (boid_histogram.max_value - boid_histogram.min_value) + boid_histogram.min_value;
        expected_histogram[i] = _strength_generator.probability(x - get_spawner_force()) * get_stats_force().size() * (boid_histogram.max_value - boid_histogram.min_value) / HISTOGRAM_SIZE;
    }

    static std::array<std::string, 4> force_names = { "Avoid", "Match", "Center", "Bias" };
    if (ImGui::BeginCombo("##forcetype", force_names[which_force].c_str())) {
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
    ImGui::PlotLines("##forces", boid_histogram.histogram.data(), HISTOGRAM_SIZE, 0, nullptr, 0, boid_histogram.max_count, ImVec2(0, 100));
    ImGui::PlotLines("##expected", expected_histogram.data(), HISTOGRAM_SIZE, 0, nullptr, 0, boid_histogram.max_count, ImVec2(0, 100));
    ImGui::Text("%f - %f", boid_histogram.min_value, boid_histogram.max_value);
    ImGui::Text("Expected = %f", boid_histogram.expected_value);
    ImGui::Text("Variance = %f", boid_histogram.variance);
}

void UiRenderer::drawStatisticsBoidSpeed(UiVariables&, BoidContainer&, BoidSpawner& spawner) {
    constexpr auto HISTOGRAM_SIZE = 9;
    auto boid_histogram = ProbabilityHistogram<glm::vec3>(HISTOGRAM_SIZE, spawner.stats().velocities, [](const glm::vec3& velocity) { return glm::length(velocity); });
    auto expected_histogram = std::vector<float>(HISTOGRAM_SIZE, 0);
    auto& _speed_generator = Variables::instance()._boid_velocity_random;
    for (size_t i = 0; i < HISTOGRAM_SIZE; i++) {
        // here, the min and max are not defined in the generator
        float x = static_cast<float>(i) / (HISTOGRAM_SIZE - 1);
        expected_histogram[i] = _speed_generator.probabilityNormalized(x) * spawner.stats().velocities.size() * (boid_histogram.max_value - boid_histogram.min_value) / HISTOGRAM_SIZE;
    }

    ImGui::PlotHistogram("##speeds", boid_histogram.histogram.data(), HISTOGRAM_SIZE, 0, nullptr, 0, boid_histogram.max_count, ImVec2(0, 100));
    ImGui::PlotHistogram("##expected", expected_histogram.data(), HISTOGRAM_SIZE, 0, nullptr, 0, boid_histogram.max_count, ImVec2(0, 100));
    ImGui::Text("%f - %f", boid_histogram.min_value, boid_histogram.max_value);
    ImGui::Text("Expected = %f", boid_histogram.expected_value);
    ImGui::Text("Variance = %f", boid_histogram.variance);
}

void UiRenderer::drawStatisticsBoidColors(UiVariables&, BoidContainer& container) {
    auto colors = Variables::instance()._boid_color_generator.values();
    for (auto& [color, _] : colors) {
        size_t count = std::count_if(container.boids().begin(), container.boids().end(), [&](const auto& boid) {
            return boid.color() == color;
        });
        std::string color_code = std::format("#{:02X}{:02X}{:02X}", static_cast<int>(255 * color.r), static_cast<int>(255 * color.g), static_cast<int>(255 * color.b));
        // ImGui::SliderInt(color_code.c_str(), reinterpret_cast<int*>(&count), 0, static_cast<int>(container.boids().size()), "%d", ImGuiSliderFlags_NoInput);
        ImGui::ColorEdit3(color_code.c_str(), &color.r);
        ImGui::ProgressBar(static_cast<float>(count) / container.boids().size(), ImVec2(0, 0));
    }
}

void UiRenderer::drawStatisticsPlanetSpawning(UiVariables& ui_variables, BoidRenderer& boid_renderer) {
    enum WhichRevolutions {
        ORBIT,
        DAYLIGHT,
    };
    static std::array<std::string, 2> revolution_names = { "Orbit", "Daylight" };
    static WhichRevolutions which_revolution = ORBIT;
    static auto get_revolution = [](const BoidRenderer::EnvironmentMesh& mesh) {
        switch (which_revolution) {
        case ORBIT:
            return mesh.orbit_cycle;
        case DAYLIGHT:
            return mesh.daylight_cycle;
        }
        return 0.f;
    };

    constexpr auto HISTOGRAM_SIZE = 9;
    auto orbits_histogram = ProbabilityHistogram<BoidRenderer::EnvironmentMesh>(HISTOGRAM_SIZE, boid_renderer.environment_meshes, get_revolution);
    static std::vector<float> expected_histogram(HISTOGRAM_SIZE, 0);
    auto& _orbit_random = Variables::instance()._renderer_orbit_random;
    for (size_t i = 0; i < HISTOGRAM_SIZE; i++) {
        float x = static_cast<float>(i) / (HISTOGRAM_SIZE - 1) * (orbits_histogram.max_value - orbits_histogram.min_value) + orbits_histogram.min_value;
        expected_histogram[i] = _orbit_random.probability(x) * boid_renderer.environment_meshes.size() * (orbits_histogram.max_value - orbits_histogram.min_value) / HISTOGRAM_SIZE;
    }
    if (ImGui::BeginCombo("##revolutiontype", revolution_names[which_revolution].c_str())) {
        if (ImGui::Selectable("Orbit", which_revolution == ORBIT)) {
            which_revolution = ORBIT;
        }
        if (ImGui::Selectable("Daylight", which_revolution == DAYLIGHT)) {
            which_revolution = DAYLIGHT;
        }
        ImGui::EndCombo();
    }
    ImGui::PlotLines("##orbits", orbits_histogram.histogram.data(), HISTOGRAM_SIZE, 0, nullptr, 0, orbits_histogram.max_count, ImVec2(0, 100));
    ImGui::PlotLines("##expected", expected_histogram.data(), HISTOGRAM_SIZE, 0, nullptr, 0, orbits_histogram.max_count, ImVec2(0, 100));
    ImGui::Text("%f - %f", orbits_histogram.min_value, orbits_histogram.max_value);
    ImGui::Text("Expected = %f", orbits_histogram.expected_value);
    ImGui::Text("Variance = %f", orbits_histogram.variance);
}

void UiRenderer::drawStatisticsBoxColors(UiVariables&, BoidRenderer& boid_renderer) {
    const auto& color_chain = boid_renderer.color_chain;
    auto colors = color_chain.values();
    auto probabilities = color_chain.nextProbabilities();

    for (size_t i = 0; i < colors.size(); i++) {
        auto& color = colors[i];
        auto& probability = probabilities[i];
        std::string color_code = std::format("#{:02X}{:02X}{:02X} ##{}", static_cast<int>(255 * color.r), static_cast<int>(255 * color.g), static_cast<int>(255 * color.b), i);
        ImGui::ColorEdit3(color_code.c_str(), &color.r);
        ImGui::ProgressBar(probability, ImVec2(0, 0));
    }
}

void UiRenderer::updateStates(UiVariables& ui_variables, BoidContainer& container, BoidSpawner& spawner, BoidRenderer& boid_renderer) {
    auto boid_spawner = [&spawner](Boid& boid) { spawner.spawnBoid(boid); };
    auto bait_spawner = [&spawner](Bait& bait) { spawner.spawnBait(bait); };

    if (ui_variables.add_amount < 0) {
        ui_variables.add_amount = 0;
    }
    if (ui_variables.spy_index >= ui_variables.amount) {
        ui_variables.spy_index = ui_variables.amount - 1;
    }
    if (ui_variables.spy_index < 0 && ui_variables.amount > 0) {
        ui_variables.spy_index = 0;
    }
    if (_flags.destroy_boids) {
        container.destroyBoids(container.boids().size());
        spawner.stats().positions.clear();
        spawner.stats().velocities.clear();
        spawner.stats().forces.clear();
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
    if (_flags.color_edit) {
        Variables::instance()._boid_color_generator.recalculateTotal();
        _flags.color_edit = false;
    }

    ui_variables.amount = static_cast<int>(container.boids().size());
}
