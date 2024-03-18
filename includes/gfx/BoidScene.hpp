#pragma once

#include "prog/BoidContainer.hpp"
#include <TotoGL/TotoGL.hpp>

class BoidScene {
public:
    BoidScene();
    ~BoidScene();

    void draw(TotoGL::Renderer&, TotoGL::Camera&, BoidContainer&);

private:
    TotoGL::RenderObjectInstanceId _appearance;
};
