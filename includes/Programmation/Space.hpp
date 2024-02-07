#pragma once

#include "Programmation/Boid.hpp"
#include <TotoGL.hpp>
#include <vector>

class Space {
public:
    Space(const size_t &nb_boids = 0);
    ~Space() = default;

    void update(const TotoGL::Seconds &);
    void display(const TotoGL::GL3RendererPtr &);

    const TotoGL::PerspectiveCameraPtr &camera() const { return _camera; }

private:
    TotoGL::ScenePtr _scene;
    TotoGL::PerspectiveCameraPtr _camera;
    TotoGL::MeshPtr _boid_mesh;
    std::vector<Boid> _boids;

    void clearScene();
    void addToScene(const Boid &);
};
