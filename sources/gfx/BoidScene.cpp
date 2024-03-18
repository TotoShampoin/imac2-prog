#include "gfx/BoidScene.hpp"
#include "prog/BoidContainer.hpp"

TotoGL::RenderObjectInstanceId createObjectInstance() {
    return TotoGL::RenderObjectFactory::create(TotoGL::RenderObject(
        TotoGL::MeshFactory::create(
            TotoGL::Mesh::cube()),
        TotoGL::ShaderMaterialFactory::create(TotoGL::ShaderMaterial(
            TotoGL::VertexShader(std::ifstream("assets/shaders/shader.vert")),
            TotoGL::FragmentShader(std::ifstream("assets/shaders/shader.frag"))))));
}

BoidScene::BoidScene()
    : _appearance(createObjectInstance()) { }

BoidScene::~BoidScene() {
    TotoGL::RenderObjectFactory::destroy(_appearance);
}

void BoidScene::draw(TotoGL::Renderer& renderer, TotoGL::Camera& camera, BoidContainer& boid_space) {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
    _appearance->material().uniform("u_color", glm::vec4(0., 0., 1., 1.));
    _appearance->mesh().cull_face() = TotoGL::Mesh::CullFace::BACK;
    _appearance->scaling() = glm::vec3(2, 2, 2) * boid_space.cubeRadius();
    _appearance->position() = { 0, 0, 0 };
    _appearance->rotation() = { 0, 0, 0 };
    renderer.render(*_appearance, camera);

    renderer.clear(false, true, false);

    _appearance->material().uniform("u_color", glm::vec4(0., 1., 1., 1.));
    _appearance->mesh().cull_face() = TotoGL::Mesh::CullFace::FRONT;
    _appearance->scaling() = { .05, .05, .05 };
    for (const auto& boid : boid_space.boids()) {
        _appearance->position() = boid.position();
        _appearance->lookAt(boid.position() + boid.velocity());
        renderer.render(*_appearance, camera);
    }

    glEnable(GL_BLEND);
    _appearance->material().uniform("u_color", glm::vec4(0., 0., 1., .25));
    _appearance->mesh().cull_face() = TotoGL::Mesh::CullFace::BACK;
    _appearance->scaling() = glm::vec3(2, 2, 2) * boid_space.cubeRadius();
    _appearance->position() = { 0, 0, 0 };
    _appearance->rotation() = { 0, 0, 0 };

    renderer.render(*_appearance, camera);
}
