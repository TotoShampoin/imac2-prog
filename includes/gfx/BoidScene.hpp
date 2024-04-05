#pragma once

#include "TotoGL/Loaders/WavefrontLoader.hpp"
#include "TotoGL/RenderObject/Camera.hpp"
#include "TotoGL/RenderObject/MaterialObject.hpp"
#include "prog/BoidContainer.hpp"
#include <TotoGL/TotoGL.hpp>
#include <vector>

class BoidScene {
public:
    BoidScene() = default;
    ~BoidScene();

    void update(const BoidContainer& container, TotoGL::Camera& camera);

    std::vector<TotoGL::MaterialObjectInstanceId>& objects() { return _boid_objects; }
    std::vector<TotoGL::MaterialObjectInstanceId>& objectsHigh() { return _boid_objects_high; }
    std::vector<TotoGL::MaterialObjectInstanceId>& objectsLow() { return _boid_objects_low; }

private:
    std::vector<TotoGL::MaterialObjectInstanceId> _boid_objects;

    std::vector<TotoGL::MaterialObjectInstanceId> _boid_objects_high, _boid_objects_low;
    TotoGL::MaterialObjectInstanceId _boid_object_base_high {
        TotoGL::MaterialObjectFactory::create(
            TotoGL::loadWavefront("assets/models/rubio/high.obj"))
    };
    TotoGL::MaterialObjectInstanceId _boid_object_base_low {
        TotoGL::MaterialObjectFactory::create(
            TotoGL::loadWavefront("assets/models/rubio/low.obj"))
    };

    void resize(const size_t& new_size);
};
