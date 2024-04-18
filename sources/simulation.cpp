#include "simulation.hpp"

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

Simulation::Simulation(TotoGL::Window& window, TotoGL::Renderer& renderer)
    : window(window)
    , boid_renderer(window, renderer) {
    initImGui(window);
    orbit.bindEvents(window, isImGuiFocused, false);
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

void Simulation::update(const TotoGL::Seconds& delta) {
    // inputs
    ui_renderer.updateStates(ui_variables, container, spawner, boid_renderer);
    glm::vec3 direction = orbit.front(true) * -player_direction.z + orbit.right(true) * player_direction.x + orbit.up(true) * player_direction.y;
    glm::vec3 push_force {};

    player.move(direction, delta);
    for (int i = 0; i < 3; i++) {
        if (player.position()[i] < -container.cubeRadius() + .5f)
            push_force[i] = 1;
        if (player.position()[i] > container.cubeRadius() - .5f)
            push_force[i] = -1;
    }
    player.push(push_force, delta);

    spawner.position() = player.position() + player.direction() * .5f;
    spawner.direction() = player.direction();

    // update
    container.updatePlayerBoid(player.position());
    container.update(delta);
    if (container.boids().empty()) {
        monitor_camera.position() = glm::vec3(container.cubeRadius() * 2);
        monitor_camera.lookAt({ 0, 0, 0 });
    } else {
        const auto& spied_boid = container.boids()[ui_variables.spy_index];
        monitor_camera.position() = spied_boid.position();
        monitor_camera.lookAt(spied_boid.position() + spied_boid.velocity());
    }
    player.update(delta);
    orbit.position() = player.position();
    orbit.apply(camera);

    auto update = timer.getDeltaTime();
    timers.clear();
    timers.emplace_back("update", update);
}

void Simulation::draw(const TotoGL::Seconds& delta) {
    monitor_texture->draw([&]() {
        boid_renderer.render(container, player, monitor_camera);
        timers.emplace_back("monitor rendering", timer.getDeltaTime());
    });

    window.draw([&]() {
        boid_renderer.render(container, player, camera);
        timers.emplace_back("scene rendering", timer.getDeltaTime());

        ui_renderer.draw(window, container, ui_variables, spawner, delta, monitor_texture);
    });
}
