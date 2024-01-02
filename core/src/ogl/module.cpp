#include <okami/ogl/module.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <im3d.h>

using namespace okami;

struct CGLRenderSurface {};

okami::GLRendererModule::GLRendererModule(GlfwModule const& module) : Module(ModuleDesc{}) {}

Error okami::GLRendererModule::CreateRenderSurface(entt::registry& registry, entity e) {
    auto glfwComponent = TryGet<GlfwWindowInstance>(registry, e);
    OKAMI_ERR_RETURN(glfwComponent);

    registry.emplace<CGLRenderSurface>(e);

    return {};
}

Error okami::GLRendererModule::Initialize(entt::registry& registry) const {
    auto windowView = registry.view<GlfwWindowInstance, CGLRenderSurface>();
    OKAMI_ERR_RETURN_IF(windowView.begin() == windowView.end(), 
        RuntimeError{"No GL render surfaces created! Cannot initialize!"});

    auto& firstWindow = windowView.template get<GlfwWindowInstance>(*windowView.begin());
    glfwMakeContextCurrent(firstWindow.window);

    int version = gladLoadGL();
    OKAMI_ERR_RETURN_IF(!version, RuntimeError{"Failed to load OpenGL!"});

    return {};
}

Error okami::GLRendererModule::PreExecute(entt::registry& registry) const {
    return {};
}

Error okami::GLRendererModule::PostExecute(entt::registry& registry) const {
    auto windowView = registry.view<GlfwWindowInstance, CGLRenderSurface>();

    for (auto e : windowView) {
        auto& window = windowView.template get<GlfwWindowInstance>(e);

        if (!window.hasFiredWindowClosed) {
            glfwMakeContextCurrent(window.window);

            glClearColor(0.3, 0.3, 0.3, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glfwSwapBuffers(window.window);
        }
    }

    return {};
}

Error okami::GLRendererModule::Destroy(entt::registry& registry) const {
    return {};
}