#include <okami/gl_renderer.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace okami;

struct CGLRenderSurface {};

okami::GLRenderer::GLRenderer(GlfwModule const& module) {}

Error okami::GLRenderer::CreateRenderSurface(entt::registry& registry, entity e) {
    auto glfwComponent = TryGet<CGlfwWindowInstance>(registry, e);
    OKAMI_ERR_RETURN(glfwComponent);

    registry.emplace<CGLRenderSurface>(e);

    return {};
}

Error okami::GLRenderer::Initialize(entt::registry& registry) const {
    Error err;
    int version = gladLoadGL();
    OKAMI_ERR_IF(!version, err, RuntimeError{"Failed to load OpenGL!"});
    OKAMI_ERR_RETURN(err);

    return {};
}

void okami::GLRenderer::PreExecute(entt::registry& registry) const {

}

void okami::GLRenderer::PostExecute(entt::registry& registry) const {

}

Error okami::GLRenderer::Destroy(entt::registry& registry) const {
    return {};
}