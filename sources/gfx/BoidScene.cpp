#include "gfx/BoidScene.hpp"
#include "prog/BoidContainer.hpp"

BoidScene::~BoidScene() {
    resize(0);
}

void BoidScene::update(const BoidContainer& container, const std::optional<std::reference_wrapper<const Boid>>& spy) {
    if (_boid_objects.size() != container.boids().size())
        resize(container.boids().size());
    for (size_t i = 0; i < container.boids().size(); ++i) {
        auto& boid = container.boids()[i];
        auto& object = *_boid_objects[i];
        object.position() = boid.position();
        object.lookAt(boid.position() + boid.velocity());
        // if (spy && &boid == &spy.value().get()) {
        //     object.setMaterial(_spy_material);
        // } else {
        //     object.setMaterial(_boid_material);
        // }
    }
}

void BoidScene::resize(const size_t& new_size) {
    const size_t old_size = _boid_objects.size();
    if (new_size > old_size) {
        _boid_objects.reserve(new_size);
        for (size_t i = old_size; i < new_size; ++i) {
            // auto boid = createObjectInstance();
            auto boid = _boid_object_base->clone();
            boid->scaling() = { .15, .15, .15 };
            _boid_objects.push_back(boid);
        }
    } else if (new_size < old_size) {
        for (size_t i = new_size; i < old_size; ++i) {
            TotoGL::MaterialObjectFactory::destroy(_boid_objects[i]);
        }
        _boid_objects.resize(new_size);
    }
}
