#pragma once
#include "TotoGL/Primitives/Clock.hpp"
#include "prog/BoidContainer.hpp"
#include "prog/BoidSpawner.hpp"
#include <TotoGL/TotoGL.hpp>
#include <cstddef>

struct Flags {
    bool changing_amount = false;
    bool destroy_boids = false;
    bool toggling_spy = false;
    bool spying_next = false;
    bool spying_previous = false;
    bool add_boid = false;
    bool remove_boid = false;
    bool add_bait = false;
    bool destroy_baits = false;
};
struct UiVariables {
    int amount = 0;
    size_t spy_index = 0;
    int add_amount = 10;
};

class UiRenderer {
public:
    void draw(
        TotoGL::Window&, BoidContainer&,
        UiVariables&, BoidSpawner&,
        const TotoGL::Seconds& delta,
        const std::vector<std::pair<std::string, float>>& timers,
        const TotoGL::BufferTextureInstanceId& monitor_texture);

    void updateStates(
        BoidContainer&,
        UiVariables&,
        BoidSpawner&);

    [[nodiscard]] Flags flags() const { return _flags; }

private:
    Flags _flags;
};
