#pragma once
#include "TotoGL/Primitives/Clock.hpp"
#include "gfx/BoidRenderer.hpp"
#include "prog/BoidContainer.hpp"
#include "prog/BoidSpawner.hpp"
#include <TotoGL/TotoGL.hpp>

struct Flags {
    bool changing_amount = false;
    bool destroy_boids = false;
    bool toggling_spy = false;
    bool changing_spy = false;
    bool add_boid = false;
    bool remove_boid = false;
    bool add_bait = false;
    bool destroy_baits = false;
    bool respawn_planets = false;
    bool color_edit = false;
};
struct UiVariables {
    int amount = 0;
    int spy_index = 0;
    int add_amount = 10;
    int planet_amount = 10;
};

class UiRenderer {
public:
    void draw(
        UiVariables&,
        BoidContainer&, BoidSpawner&, BoidRenderer&,
        TotoGL::Window&, const TotoGL::Seconds& delta,
        const TotoGL::BufferTextureInstanceId& monitor_texture);

    void updateStates(UiVariables&, BoidContainer&, BoidSpawner&, BoidRenderer&);

    [[nodiscard]] Flags flags() const { return _flags; }

private:
    Flags _flags;

    void drawSpawnControls(UiVariables&, BoidSpawner&);
    void drawGeneralControls(UiVariables&, BoidContainer&);
    void drawSpyControls(UiVariables&, BoidContainer&, const TotoGL::BufferTextureInstanceId&);
    void drawStatistics(UiVariables&, BoidContainer&, BoidSpawner&, BoidRenderer&, const TotoGL::Seconds& delta);

    void drawStatisticsBoidSpawning(UiVariables&, BoidContainer&, BoidSpawner&);
    void drawStatisticsBoidForces(UiVariables&, BoidContainer&, BoidSpawner&);
    void drawStatisticsBoidSpeed(UiVariables&, BoidContainer&, BoidSpawner&);
    void drawStatisticsBoidColors(UiVariables&, BoidContainer&);
    void drawStatisticsPlanetSpawning(UiVariables&, BoidRenderer&);
};
