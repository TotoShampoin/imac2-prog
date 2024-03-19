#include "gfx/BoidScene.hpp"
#include "prog/BoidContainer.hpp"

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

BoidScene::BoidScene()
    : _appearance(createCubeInstance())
    , _territory(createSphereInstance()) {
    (*_territory).mesh().draw_method() = TotoGL::Mesh::DrawMethod::LINES;
}

BoidScene::~BoidScene() {
    TotoGL::RenderObjectFactory::destroy(_appearance);
    TotoGL::RenderObjectFactory::destroy(_territory);
}

void BoidScene::draw(TotoGL::Renderer& renderer, TotoGL::Camera& camera, const BoidContainer& container, const std::optional<std::reference_wrapper<const Boid>>& spy) {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);
    _appearance->material().uniform("u_color", glm::vec4(0., 0., 1., 1.));
    _appearance->mesh().cull_face() = TotoGL::Mesh::CullFace::BACK;
    _appearance->scaling() = glm::vec3(2, 2, 2) * container.cubeRadius();
    _appearance->position() = { 0, 0, 0 };
    _appearance->rotation() = { 0, 0, 0 };
    renderer.render(*_appearance, camera);

    renderer.clear(false, true, false);

    _appearance->mesh().cull_face() = TotoGL::Mesh::CullFace::FRONT;
    _appearance->scaling() = { .05, .05, .05 };
    for (const auto& boid : container.boids()) {
        if (spy) {
            if (&boid == &spy.value().get()) {
                glEnable(GL_BLEND);
                _territory->position() = boid.position();
                _territory->material().uniform("u_color", glm::vec4(0., 1., 0., .25));
                _territory->scaling() = glm::vec3(1) * (container.attractRadius() + container.expellRadius());
                renderer.render(*_territory, camera);
                _territory->material().uniform("u_color", glm::vec4(1., 0., 0., .25));
                _territory->scaling() = glm::vec3(1) * container.expellRadius();
                renderer.render(*_territory, camera);
                glDisable(GL_BLEND);
                _appearance->material().uniform("u_color", glm::vec4(1., 1., 1., 1.));
            } else {
                auto attract = boid.closeness(*spy, container.attractRadius(), container.expellRadius()) * 2;
                auto expell = boid.closeness(*spy, container.expellRadius(), 0) * 2;
                _appearance->material().uniform("u_color", glm::vec4(expell, attract - expell, 0., 1.));
            }
        } else {
            _appearance->material().uniform("u_color", glm::vec4(0., 1., 1., 1.));
        }
        _appearance->position() = boid.position();
        _appearance->lookAt(boid.position() + boid.velocity());
        renderer.render(*_appearance, camera);
    }

    glEnable(GL_BLEND);
    _appearance->material().uniform("u_color", glm::vec4(0., 0., 1., .25));
    _appearance->mesh().cull_face() = TotoGL::Mesh::CullFace::BACK;
    _appearance->scaling() = glm::vec3(2, 2, 2) * container.cubeRadius();
    _appearance->position() = { 0, 0, 0 };
    _appearance->rotation() = { 0, 0, 0 };

    renderer.render(*_appearance, camera);
}
