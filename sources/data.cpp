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

    boid_mesh_low->scaling() = glm::vec3(.15);
    boid_mesh_high->scaling() = glm::vec3(.15);
}

void Data::update(const TotoGL::Seconds& delta) {
    container.update(delta);
    auto container_update = timer.getDeltaTime();
    auto& spied_boid = container.boids()[spy_index];
    monitor_camera.position() = spied_boid.position() - glm::normalize(spied_boid.velocity()) * 2.f;
    monitor_camera.lookAt(spied_boid.position());
    bound_mesh->scaling() = { container.cubeRadius() + .15, container.cubeRadius() + .15, container.cubeRadius() + .15 };
    auto scene_update = timer.getDeltaTime();
    orbit.apply(camera);
    auto orbit_update = timer.getDeltaTime();

    // boid_scene.update(container, camera);

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
    auto input_update = timer.getDeltaTime();
    timers.clear();
    timers.push_back({ "boids update", container_update });
    timers.push_back({ "scene update", scene_update });
    timers.push_back({ "orbit update", orbit_update });
    timers.push_back({ "input update", input_update });
}

void Data::draw(const TotoGL::Seconds& delta) {
    global_light->setDirection({ 1, -1, 1 });

    // TODO(Rendering) Figure out why it's faster with the normal renderer than with the custom one
    // even though the custom one is supposed to skip a lot of boids overhead...
    static const auto custom_render = [&](TotoGL::Camera& cam) {
        static auto lights = std::vector<TotoGL::LightInstanceId>({ ambient_light, global_light });
        renderer.clear();
        renderer.render(skydome->object(), cam);
        renderer.clear(false, true, false);
        renderer.render(*bound_mesh, cam, lights);
        for (auto& boid : container.boids()) {
            if (glm::distance(boid.position(), cam.position()) < 5) {
                boid_mesh_high->position() = boid.position();
                boid_mesh_high->lookAt(boid.position() + boid.velocity());
                renderer.render(*boid_mesh_high, cam, lights);
            } else {
                boid_mesh_low->position() = boid.position();
                boid_mesh_low->lookAt(boid.position() + boid.velocity());
                renderer.render(*boid_mesh_low, cam, lights);
            }
        }
    };

    // scene.clear();
    // auto& boid_objects = boid_scene.objects();
    // for (auto& object : boid_objects) {
    //     scene.add(object);
    // }
    // scene.add(ambient_light);
    // scene.add(global_light);
    // scene.add(bound_mesh);
    // scene.add(skydome);
    // timers.push_back({ "scene preparation", timer.getDeltaTime() });

    monitor_texture->draw([&]() {
        // renderer.render(scene, monitor_camera);
        custom_render(monitor_camera);
        timers.push_back({ "monitor rendering", timer.getDeltaTime() });
    });

    window.draw([&]() {
        // renderer.render(scene, camera);
        custom_render(camera);
        timers.push_back({ "scene rendering", timer.getDeltaTime() });

        renderImGui([&]() {
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
