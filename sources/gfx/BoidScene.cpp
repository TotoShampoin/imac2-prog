#include "gfx/BoidScene.hpp"
#include "prog/BoidContainer.hpp"
#include <fstream>

TotoGL::RenderObjectInstanceId createObjectInstance() {
    return TotoGL::RenderObjectFactory::create(TotoGL::RenderObject(
        TotoGL::MeshFactory::create(
            TotoGL::loadWavefrontObj(std::ifstream("assets/models/fish.obj"))),
        TotoGL::ShaderMaterialFactory::create(TotoGL::ShaderMaterial(
            TotoGL::VertexShader(std::ifstream("assets/shaders/shader.vert")),
            TotoGL::FragmentShader(std::ifstream("assets/shaders/shader.frag"))))));
}
TotoGL::RenderObjectInstanceId createCubeInstance() {
    return TotoGL::RenderObjectFactory::create(TotoGL::RenderObject(
        TotoGL::MeshFactory::create(
            TotoGL::Mesh::cube()),
        TotoGL::ShaderMaterialFactory::create(TotoGL::ShaderMaterial(
            TotoGL::VertexShader(std::ifstream("assets/shaders/shader.vert")),
            TotoGL::FragmentShader(std::ifstream("assets/shaders/shader.frag"))))));
}
TotoGL::RenderObjectInstanceId createSphereInstance() {
    return TotoGL::RenderObjectFactory::create(TotoGL::RenderObject(
        TotoGL::MeshFactory::create(
            TotoGL::Mesh::sphere(1, 32, 32)),
        TotoGL::ShaderMaterialFactory::create(TotoGL::ShaderMaterial(
            TotoGL::VertexShader(std::ifstream("assets/shaders/shader.vert")),
            TotoGL::FragmentShader(std::ifstream("assets/shaders/shader.frag"))))));
}

BoidRenderer::BoidRenderer()
    : _boid_mesh(createObjectInstance())
    , _bound_mesh(createCubeInstance())
    , _territory_mesh(createSphereInstance()) {
    (*_territory_mesh).mesh().draw_method() = TotoGL::Mesh::DrawMethod::LINES;
}

BoidRenderer::~BoidRenderer() {
    TotoGL::RenderObjectFactory::destroy(_boid_mesh);
    TotoGL::RenderObjectFactory::destroy(_territory_mesh);
}

void BoidRenderer::draw(TotoGL::Renderer& renderer, TotoGL::Camera& camera, const BoidContainer& container, const std::optional<std::reference_wrapper<const Boid>>& spy) {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
    _bound_mesh->material().uniform("u_color", glm::vec4(0., 0., 1., 1.));
    _bound_mesh->mesh().cull_face() = TotoGL::Mesh::CullFace::BACK;
    _bound_mesh->scaling() = glm::vec3(2, 2, 2) * container.cubeRadius();
    _bound_mesh->position() = { 0, 0, 0 };
    _bound_mesh->rotation() = { 0, 0, 0 };
    renderer.render(*_bound_mesh, camera);

    renderer.clear(false, true, false);

    _boid_mesh->mesh().cull_face() = TotoGL::Mesh::CullFace::FRONT;
    _boid_mesh->scaling() = { .05, .05, .05 };
    for (const auto& boid : container.boids()) {
        if (spy) {
            if (&boid == &spy.value().get()) {
                glEnable(GL_BLEND);
                _territory_mesh->position() = boid.position();
                _territory_mesh->material().uniform("u_color", glm::vec4(0., 1., 0., .25));
                _territory_mesh->scaling() = glm::vec3(1) * (container.attractRadius() + container.expellRadius());
                renderer.render(*_territory_mesh, camera);
                _territory_mesh->material().uniform("u_color", glm::vec4(1., 0., 0., .25));
                _territory_mesh->scaling() = glm::vec3(1) * container.expellRadius();
                renderer.render(*_territory_mesh, camera);
                glDisable(GL_BLEND);
                _boid_mesh->material().uniform("u_color", glm::vec4(1., 1., 1., 1.));

                _bound_mesh->material().uniform("u_color", glm::vec4(0., 1., 1., 1.));
                _bound_mesh->mesh().cull_face() = TotoGL::Mesh::CullFace::FRONT;
                _bound_mesh->scaling() = glm::vec3(.1);
                for (const auto& projection : container.projectionsOnCube(boid.position())) {
                    _bound_mesh->position() = projection;
                    renderer.render(*_bound_mesh, camera);
                }
            } else {
                auto attract = boid.closeness(*spy, container.attractRadius(), container.expellRadius()) * 2;
                auto expell = boid.closeness(*spy, container.expellRadius(), 0) * 2;
                _boid_mesh->material().uniform("u_color", glm::vec4(expell, attract - expell, 0., 1.));
            }
        } else {
            _boid_mesh->material().uniform("u_color", glm::vec4(0., 1., 1., 1.));
        }
        _boid_mesh->position() = boid.position();
        _boid_mesh->lookAt(boid.position() + boid.velocity());
        renderer.render(*_boid_mesh, camera);
    }

    glEnable(GL_BLEND);
    _bound_mesh->material().uniform("u_color", glm::vec4(0., 0., 1., .25));
    _bound_mesh->mesh().cull_face() = TotoGL::Mesh::CullFace::BACK;
    _bound_mesh->scaling() = glm::vec3(2, 2, 2) * container.cubeRadius();
    _bound_mesh->position() = { 0, 0, 0 };
    renderer.render(*_bound_mesh, camera);
}
