#include "data.hpp"

#include "prog/BoidContainer.hpp"
#include "prog/imgui-impl.hpp"

#include <TotoGL/TotoGL.hpp>
#include <imgui.h>

Data::Data(TotoGL::Window& window, TotoGL::Renderer& renderer)
    : window(window)
    , renderer(renderer) {
    window.on(TotoGL::VectorEventName::WINDOW_SIZE, [&](const TotoGL::VectorEvent& event) {
        glViewport(0, 0, event.x, event.y);
        camera.setPersective(FOV, event.x / event.y, NEAR, FAR);
    });
    orbit.bindEvents(window, isImGuiFocused);
    initImGui(window);

    monitor->material().uniform("u_texture", monitor_texture->texture());
    monitor->mesh().cull_face() = TotoGL::Mesh::CullFace::BACK;

    bounds->material().uniform("u_color", glm::vec4(1, 1, 1, 1));
    bounds->mesh().cull_face() = TotoGL::Mesh::CullFace::BACK;
}

void Data::update(const TotoGL::Seconds& delta) {
    container.update(delta);
    auto container_update = timer.getDeltaTime();
    // if (spy) {
    //     orbit.position() = container.boids()[spy_index].position();
    //     boid_scene.update(container, container.boids()[spy_index]);
    // } else {
    boid_scene.update(container);
    // }
    auto& spied_boid = container.boids()[spy_index];
    monitor_camera.position() = spied_boid.position() - glm::normalize(spied_boid.velocity()) * 2.f;
    monitor_camera.lookAt(spied_boid.position());
    bounds->scaling() = { container.cubeRadius() * 2, container.cubeRadius() * 2, container.cubeRadius() * 2 };
    auto scene_update = timer.getDeltaTime();
    orbit.apply(camera);
    auto orbit_update = timer.getDeltaTime();

    if (changing_amount) {
        container.resize(amount);
        changing_amount = false;
    }
    if (resetting) {
        container.resetBoids();
        resetting = false;
    }
    // if (toggling_spy) {
    //     if (spy) {
    //         orbit.distance() = 1;
    //     } else {
    //         orbit.distance() = 10;
    //         orbit.position() = { 0, 0, 0 };
    //     }
    //     toggling_spy = false;
    // }
    if (spying_next) {
        spy_index = (spy_index + 1) % container.boids().size();
        spying_next = false;
    }
    if (spying_previous) {
        spy_index = (spy_index + container.boids().size() - 1) % container.boids().size();
        spying_previous = false;
    }
    auto input_update = timer.getDeltaTime();
    timers.clear();
    timers.push_back({ "boids update", container_update });
    timers.push_back({ "scene update", scene_update });
    timers.push_back({ "orbit update", orbit_update });
    timers.push_back({ "input update", input_update });
}

void Data::draw(const TotoGL::Seconds& delta) {
    global_light->setDirection({ 1, -1, 1 });
    monitor->position() = { container.cubeRadius(), 0, 0 };
    // monitor->lookAt(-camera.position());

    scene.clear();
    auto& boid_objects = boid_scene.objects();
    for (auto& object : boid_objects) {
        scene.add(object);
    }
    scene.add(ambient_light);
    scene.add(global_light);
    scene.add(bounds);
    timers.push_back({ "scene preparation", timer.getDeltaTime() });

    monitor_texture->draw([&]() {
        renderer.clear();
        renderer.render(scene, monitor_camera);
        timers.push_back({ "monitor rendering", timer.getDeltaTime() });
    });
    scene.add(monitor);

    window.draw([&]() {
        renderer.clear();
        renderer.render(scene, camera);
        timers.push_back({ "scene rendering", timer.getDeltaTime() });

        renderImGui([&]() {
            float h;
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
            h = ImGui::GetWindowHeight();
            ImGui::End();

            ImGui::SetNextWindowPos(ImVec2(0, window.size()[1] - h), ImGuiCond_Always, ImVec2(0, 1));
            ImGui::SetNextWindowSize(ImVec2(304, 0), ImGuiCond_Always);
            ImGui::Begin("Spy");
            // toggling_spy |= ImGui::Checkbox("Spy", &spy);
            // if (spy) {
            spying_previous |= ImGui::Button("-");
            ImGui::SameLine();
            ImGui::Text("%zu", spy_index);
            ImGui::SameLine();
            spying_next |= ImGui::Button("+");

            const auto& boid = container.boids()[spy_index];
            ImGui::Text("Position = %f, %f, %f", boid.position().x, boid.position().y, boid.position().z);
            ImGui::Text("Velocity = %f, %f, %f", boid.velocity().x, boid.velocity().y, boid.velocity().z);
            // }
            ImGui::End();

            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::Begin("Timers");
            for (const auto& [name, time] : timers) {
                ImGui::Text("%s = %fs", name.c_str(), time);
            }
            ImGui::Text("Frame time = %fs | %.0fHz", delta, 1 / delta);
            ImGui::End();
        });
    });
}
