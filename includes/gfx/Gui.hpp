#pragma once
#include "TotoGL/Primitives/Clock.hpp"
#include "prog/BoidContainer.hpp"
#include <TotoGL/TotoGL.hpp>
#include <cstddef>

struct Flags {
    bool changing_amount = false;
    bool resetting = false;
    bool toggling_spy = false;
    bool spying_next = false;
    bool spying_previous = false;
};

class Gui {
public:
    void draw(
        TotoGL::Window&, BoidContainer&,
        int& amount, const size_t& spy_index,
        const TotoGL::Seconds& delta,
        const std::vector<std::pair<std::string, float>>& timers,
        const TotoGL::BufferTextureInstanceId& monitor_texture);

    void updateStates(
        BoidContainer&,
        int& amount, size_t& spy_index);

    [[nodiscard]] Flags flags() const { return _flags; }

private:
    Flags _flags;
};
