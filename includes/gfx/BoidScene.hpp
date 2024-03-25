#pragma once

#include "TotoGL/RenderObject/Mesh.hpp"
#include "TotoGL/RenderObject/RenderObject.hpp"
#include "TotoGL/RenderObject/ShaderMaterial.hpp"
#include "prog/BoidContainer.hpp"
#include <TotoGL/TotoGL.hpp>
#include <functional>
#include <optional>
#include <vector>

class BoidRenderer {
public:
    BoidRenderer();
    ~BoidRenderer();

    void draw(TotoGL::Renderer&, TotoGL::Camera&, const BoidContainer&, const std::optional<std::reference_wrapper<const Boid>>& spy = std::nullopt);

private:
    TotoGL::RenderObjectInstanceId _boid_mesh;
    TotoGL::RenderObjectInstanceId _bound_mesh;
    TotoGL::RenderObjectInstanceId _territory_mesh;
};

class BoidScene {
public:
    BoidScene() {
        _boid_material->uniform("u_use_texture", false);
        _boid_material->uniform("u_color", glm::vec4(1., 1., 1., 1.));
        _spy_material->uniform("u_use_texture", false);
        _spy_material->uniform("u_color", glm::vec4(1., 0., 0., 1.));
    }
    ~BoidScene() {
        TotoGL::MeshFactory::destroy(_boid_mesh);
        TotoGL::ShaderMaterialFactory::destroy(_boid_material);
    }

    void update(const BoidContainer& container, const std::optional<std::reference_wrapper<const Boid>>& spy = std::nullopt) {
        if (_boid_objects.size() != container.boids().size())
            resize(container.boids().size());
        for (size_t i = 0; i < container.boids().size(); ++i) {
            auto& boid = container.boids()[i];
            auto& object = *_boid_objects[i];
            object.position() = boid.position();
            object.lookAt(boid.position() + boid.velocity());
            if (spy && &boid == &spy.value().get()) {
                object.setMaterial(_spy_material);
            } else {
                object.setMaterial(_boid_material);
            }
        }
    }

    std::vector<TotoGL::RenderObjectInstanceId>& objects() { return _boid_objects; }

private:
    std::vector<TotoGL::RenderObjectInstanceId> _boid_objects;

    TotoGL::MeshInstanceId _boid_mesh { TotoGL::MeshFactory::create(
        TotoGL::loadWavefrontObj(std::ifstream("assets/models/fish.obj"))) };

    TotoGL::ShaderMaterialInstanceId _boid_material { TotoGL::ShaderMaterialFactory::create(
        TotoGL::ShaderMaterial(
            TotoGL::VertexShader(std::ifstream("assets/shaders/shader.vert")),
            TotoGL::FragmentShader(std::ifstream("assets/shaders/phong.frag")))) };

    TotoGL::ShaderMaterialInstanceId _spy_material { TotoGL::ShaderMaterialFactory::create(
        TotoGL::ShaderMaterial(
            TotoGL::VertexShader(std::ifstream("assets/shaders/shader.vert")),
            TotoGL::FragmentShader(std::ifstream("assets/shaders/phong.frag")))) };

    void resize(const size_t& new_size) {
        const size_t old_size = _boid_objects.size();
        if (new_size > old_size) {
            _boid_objects.reserve(new_size);
            for (size_t i = old_size; i < new_size; ++i) {
                auto boid = TotoGL::RenderObjectFactory::create(
                    TotoGL::RenderObject(_boid_mesh, _boid_material));
                boid->material().uniform("u_use_texture", false);
                boid->scaling() = { .05, .05, .05 };
                _boid_objects.push_back(boid);
            }
        } else if (new_size < old_size) {
            for (size_t i = new_size; i < old_size; ++i) {
                TotoGL::RenderObjectFactory::destroy(_boid_objects[i]);
            }
            _boid_objects.resize(new_size);
        }
    }
};
