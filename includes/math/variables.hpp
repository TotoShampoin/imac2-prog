#pragma once

#include "random/enumerated.hpp"
#include "random/exponential.hpp"
#include "random/normal.hpp"
#include "random/uniform.hpp"
#include <TotoGL/Primitives/Color.hpp>

constexpr TotoGL::ColorRGB RED { 1, 0, 0 };
constexpr TotoGL::ColorRGB GREEN { 0, 1, 0 };
constexpr TotoGL::ColorRGB YELLOW { 1, 1, 0 };
constexpr TotoGL::ColorRGB BLUE { 0, 0, 1 };
constexpr TotoGL::ColorRGB MAGENTA { 1, 0, 1 };
constexpr TotoGL::ColorRGB CYAN { 0, 1, 1 };

class Variables {
public:
    Variables(const Variables&) = delete;
    Variables& operator=(const Variables&) = delete;
    Variables(Variables&&) = delete;
    Variables& operator=(Variables&&) = delete;

    static Variables& instance() {
        static Variables instance;
        return instance;
    }

    Random::Uniform<float> _boid_position_generator { -1.f, 1.f };
    Random::Normal<float> _boid_angle_generator { 0.f, .25f };
    Random::Normal<float> _boid_strength_generator { 0.f, .05f };
    Random::Enumerated<TotoGL::ColorRGB> _boid_color_generator { {
        { YELLOW, 5 },
        { GREEN, 1 },
        { RED, 1 },
        { BLUE, 1 },
        { MAGENTA, 1 },
        { CYAN, 1 },
    } };

    Random::Uniform<float> _renderer_index_random { 0, 1 };
    Random::Uniform<float> _renderer_position_random { -25, 25 };
    Random::Exponential<float> _renderer_orbit_random { 1 };

private:
    Variables() = default;
    ~Variables() = default;
};
