#include "gfx/BoidRenderer.hpp"

RendererObjects::RendererObjects()
    : lights({
        TotoGL::LightFactory::create(
            TotoGL::Light({ .25, .5, 1 }, .2, TotoGL::LightType::AMBIENT)),
        TotoGL::LightFactory::create(
            TotoGL::Light({ 1, 1, 1 }, 1, TotoGL::LightType::DIRECTIONAL)),
        TotoGL::LightFactory::create(
            TotoGL::Light({ 1, .6, 0 }, 3, TotoGL::LightType::POINT)),
    })

    , skydome_texture(TotoGL::TextureFactory::create(
          TotoGL::Texture(std::ifstream("assets/textures/skydome.jpg"))))
    , skydome(TotoGL::SkydomeFactory::create(
          TotoGL::Skydome(*skydome_texture)))

    , cube_texture(TotoGL::TextureFactory::create(
          TotoGL::Texture(std::ifstream("assets/textures/noise.jpg"))))
    , cube_blend_texture(TotoGL::TextureFactory::create(
          TotoGL::Texture(std::ifstream("assets/textures/noise_blend.png"))))
    , cube_mesh(TotoGL::RenderObjectFactory::create(
          TotoGL::RenderObject(
              TotoGL::MeshFactory::create(
                  TotoGL::Mesh::cube()),
              TotoGL::ShaderMaterialFactory::create(
                  TotoGL::ShaderMaterial(
                      std::ifstream("assets/shaders/shader.vert"),
                      std::ifstream("assets/shaders/cube.frag"))))))

    , boid_mesh_high(TotoGL::MaterialObjectFactory::create(
          TotoGL::loadWavefront("assets/models/rubio/high.obj")))
    , boid_mesh_low(TotoGL::MaterialObjectFactory::create(
          TotoGL::loadWavefront("assets/models/rubio/low.obj")))

    , shadow_mesh(TotoGL::RenderObjectFactory::create(
          TotoGL::RenderObject(
              TotoGL::MeshFactory::create(
                  TotoGL::Mesh::quad(.5, .5)),
              TotoGL::ShaderMaterialFactory::create(
                  TotoGL::ShaderMaterial(
                      std::ifstream("assets/shaders/shader.vert"),
                      std::ifstream("assets/shaders/shadow.frag"))))))

    , player_mesh(TotoGL::MaterialObjectFactory::create(
          TotoGL::loadWavefront("assets/models/boyo/boyo.obj")))

    , bait_mesh(TotoGL::MaterialObjectFactory::create(
          TotoGL::loadWavefront("assets/models/gem.obj")))

    , world_meshes({
          TotoGL::MaterialObjectFactory::create(
              TotoGL::loadWavefront("assets/models/planet/planet.obj")),
          TotoGL::MaterialObjectFactory::create(
              TotoGL::loadWavefront("assets/models/planet/planet2.obj")),
          TotoGL::MaterialObjectFactory::create(
              TotoGL::loadWavefront("assets/models/planet/rock.obj")),
      }) { }

RendererObjects::~RendererObjects() {
    for (auto& light : lights) {
        TotoGL::LightFactory::destroy(light);
    }
    TotoGL::SkydomeFactory::destroy(skydome);
    TotoGL::TextureFactory::destroy(skydome_texture);
    TotoGL::TextureFactory::destroy(cube_texture);
    TotoGL::TextureFactory::destroy(cube_blend_texture);
    TotoGL::RenderObjectFactory::destroy(cube_mesh);
    TotoGL::MaterialObjectFactory::destroy(boid_mesh_high);
    TotoGL::MaterialObjectFactory::destroy(boid_mesh_low);
    TotoGL::MaterialObjectFactory::destroy(player_mesh);
    TotoGL::MaterialObjectFactory::destroy(bait_mesh);
    for (auto& world_mesh : world_meshes) {
        TotoGL::MaterialObjectFactory::destroy(world_mesh);
    }
}
