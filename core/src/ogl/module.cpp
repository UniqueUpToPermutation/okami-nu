#include <okami/ogl/module.hpp>
#include <okami/ogl/renderer.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <im3d.h>

#include <okami/log.hpp>

using namespace okami;

struct GLRenderSurface {};

okami::GLRendererModule::GLRendererModule(GlfwModule const& module) : Module(ModuleDesc{}) {}

Error okami::GLRendererModule::CreateRenderSurface(entt::registry& registry, entity e) {
    auto glfwComponent = TryGet<GlfwWindowInstance>(registry, e);
    OKAMI_ERR_RETURN(glfwComponent);

    registry.emplace<GLRenderSurface>(e);

    return {};
}

Error okami::GLRendererModule::Initialize(entt::registry& registry) const {
    PLOG_INFO << "Initializing GLRenderer...";

    auto windowView = registry.view<GlfwWindowInstance, GLRenderSurface>();
    OKAMI_ERR_RETURN_IF(windowView.begin() == windowView.end(), 
        RuntimeError{"No GL render surfaces created! Cannot initialize!"});

    auto& firstWindow = windowView.template get<GlfwWindowInstance>(*windowView.begin());
    glfwMakeContextCurrent(firstWindow.window);

    int version = gladLoadGL();
    OKAMI_ERR_RETURN_IF(!version, RuntimeError{"Failed to load OpenGL!"});

    auto& renderer = registry.ctx().emplace<GLRenderer>();
    auto& im3dContext = registry.ctx().emplace<Im3d::Context>();

    return {};
}

Error okami::GLRendererModule::PreExecute(entt::registry& registry) const {
    return {};
}

Error okami::GLRendererModule::PostExecute(entt::registry& registry) const {
    auto windowView = registry.view<GlfwWindowInstance, GLRenderSurface>();

    for (auto e : windowView) {
        auto& window = windowView.template get<GlfwWindowInstance>(e);

        if (!window.hasFiredWindowClosed) {
            glfwMakeContextCurrent(window.window);

            auto& renderer = registry.ctx().get<GLRenderer>();
            auto& im3d = registry.ctx().get<Im3d::Context>();

            auto windowSize = GetProperty<WindowSize>(registry, e)
                .value_or(WindowSize{100, 100});
            auto cameraEntity = GetProperty<CameraReference>(registry, e)
                .value_or(CameraReference{}).entity;

            auto camera = GetProperty<Camera>(registry, cameraEntity).value_or(Camera{});
            auto transform = GetProperty<Transform>(registry, cameraEntity).value_or(Transform{});

            CameraRenderData renderCamera {
                .camera = camera,
                .viewport = windowSize.AsVec2(),
                .viewTransform = Inverse(transform)
            };

            renderer.Draw(renderCamera, im3d);

            glfwSwapBuffers(window.window);
        }
    }

    return {};
}

Error okami::GLRendererModule::Destroy(entt::registry& registry) const {
    PLOG_INFO << "Shutting down GLRenderer...";
    return {};
}