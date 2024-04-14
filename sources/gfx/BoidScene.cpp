#include "gfx/BoidScene.hpp"
#include "prog/BoidContainer.hpp"

BoidScene::~BoidScene() {
    resize(0);
}

void BoidScene::update(const BoidContainer& container, TotoGL::Camera& camera) {
    if (_boid_objects.size() != container.boids().size()) {
        resize(container.boids().size());
    }
    _boid_objects.clear();
    for (size_t i = 0; i < container.boids().size(); ++i) {
        const auto& boid = container.boids()[i];
        if (glm::distance(boid.position(), camera.position()) < 5) {
            _boid_objects.push_back(_boid_objects_high[i]);
        } else {
            _boid_objects.push_back(_boid_objects_low[i]);
        }
        _boid_objects_high[i]->position() = boid.position();
        _boid_objects_high[i]->lookAt(boid.position() + boid.velocity());
        _boid_objects_low[i]->position() = boid.position();
        _boid_objects_low[i]->lookAt(boid.position() + boid.velocity());
    }
}

void BoidScene::resize(const size_t& new_size) {
    const size_t old_size = _boid_objects.size();
    if (new_size > old_size) {
        _boid_objects_high.reserve(new_size);
        _boid_objects_low.reserve(new_size);
        for (size_t i = old_size; i < new_size; ++i) {
            auto boid_high = _boid_object_base_high->clone();
            boid_high->scaling() = { .15, .15, .15 };
            _boid_objects_high.push_back(boid_high);

            auto boid_low = _boid_object_base_low->clone();
            boid_low->scaling() = { .15, .15, .15 };
            _boid_objects_low.push_back(boid_low);
        }
    } else if (new_size < old_size) {
        for (size_t i = new_size; i < old_size; ++i) {
            TotoGL::MaterialObjectFactory::destroy(_boid_objects_high[i]);
            TotoGL::MaterialObjectFactory::destroy(_boid_objects_low[i]);
        }
        _boid_objects_high.resize(new_size);
        _boid_objects_low.resize(new_size);
    }
}
