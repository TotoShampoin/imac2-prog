#pragma once

#include "TotoGL/RenderObject/RenderObject.hpp"
#include "prog/BoidContainer.hpp"
#include <TotoGL/TotoGL.hpp>
#include <functional>
#include <optional>

class BoidScene {
public:
    BoidScene();
    ~BoidScene();

    void draw(TotoGL::Renderer&, TotoGL::Camera&, const BoidContainer&, const std::optional<std::reference_wrapper<const Boid>>& spy = std::nullopt);

private:
    TotoGL::RenderObjectInstanceId _boid_mesh;
    TotoGL::RenderObjectInstanceId _bound_mesh;
    TotoGL::RenderObjectInstanceId _territory_mesh;
};
