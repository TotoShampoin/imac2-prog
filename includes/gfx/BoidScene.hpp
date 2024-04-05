#pragma once

#include "TotoGL/Loaders/WavefrontLoader.hpp"
#include "TotoGL/RenderObject/MaterialObject.hpp"
#include "prog/BoidContainer.hpp"
#include <TotoGL/TotoGL.hpp>
#include <functional>
#include <optional>
#include <vector>

class BoidScene {
public:
    BoidScene() = default;
    ~BoidScene();

    void update(const BoidContainer& container, const std::optional<std::reference_wrapper<const Boid>>& spy = std::nullopt);

    // std::vector<TotoGL::RenderObjectInstanceId>& objects() { return _boid_objects; }
    std::vector<TotoGL::MaterialObjectInstanceId>& objects() { return _boid_objects; }

private:
    // std::vector<TotoGL::RenderObjectInstanceId> _boid_objects;
    std::vector<TotoGL::MaterialObjectInstanceId> _boid_objects;
    TotoGL::MaterialObjectInstanceId _boid_object_base {
        TotoGL::MaterialObjectFactory::create(
            TotoGL::loadWavefront("assets/models/rubio/lower.obj"))
    };

    // TotoGL::MeshInstanceId _boid_mesh { TotoGL::MeshFactory::create(
    //     TotoGL::loadWavefrontObj(std::ifstream("assets/models/fish.obj"))) };

    // TotoGL::ShaderMaterialInstanceId _boid_material { TotoGL::ShaderMaterialFactory::create(
    //     TotoGL::ShaderMaterial(
    //         TotoGL::VertexShader(std::ifstream("assets/shaders/shader.vert")),
    //         TotoGL::FragmentShader(std::ifstream("assets/shaders/phong.frag")))) };

    // TotoGL::ShaderMaterialInstanceId _spy_material { TotoGL::ShaderMaterialFactory::create(
    //     TotoGL::ShaderMaterial(
    //         TotoGL::VertexShader(std::ifstream("assets/shaders/shader.vert")),
    //         TotoGL::FragmentShader(std::ifstream("assets/shaders/phong.frag")))) };

    void resize(const size_t& new_size);
};
