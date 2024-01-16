#include <okami/ogl/module.hpp>
#include <okami/ogl/renderer.hpp>

#include <okami/window.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <im3d.h>

#include <plog/Log.h>

using namespace okami;

struct GLRenderSurface {};

okami::GLRendererModule::GLRendererModule(GlfwModule const& module) : Module(ModuleDesc{}) {}

void okami::GLRendererModule::RegisterPrototypes(std::unordered_map<std::string, Prototype>& proto) const {
    // Window prototype
    proto[std::string{prototypes::Window}].factories.emplace_back([](Registry& reg, entity e) -> Error {
        auto glfwComponent = TryGet<GlfwWindowInstance>(reg, e);
        OKAMI_ERR_RETURN(glfwComponent);

        reg.emplace<GLRenderSurface>(e);
        return {};
    });
}

Error okami::GLRendererModule::Initialize(Registry& registry) const {
    PLOG_INFO << "Initializing GLRenderer...";

    auto windowView = registry.view<GlfwWindowInstance, GLRenderSurface>();
    OKAMI_ERR_RETURN_IF(windowView.begin() == windowView.end(), 
        RuntimeError{"No GL render surfaces created! Cannot initialize!"});

    auto& firstWindow = windowView.template get<GlfwWindowInstance>(*windowView.begin());
    glfwMakeContextCurrent(firstWindow.window);

    int version = gladLoadGL();
    OKAMI_ERR_RETURN_IF(!version, RuntimeError{"Failed to load OpenGL!"});

    auto& renderer = registry.ctx().emplace<GLRenderer>();

    return {};
}

Error okami::GLRendererModule::PreExecute(Registry& registry) const {
    return {};
}

Error okami::GLRendererModule::PostExecute(Registry& registry) const {
    Error err;
    auto windowView = registry.view<GlfwWindowInstance, GLRenderSurface>();

    for (auto e : windowView) {
        auto& window = windowView.template get<GlfwWindowInstance>(e);

        if (!window.hasFiredWindowClosed) {
            glfwMakeContextCurrent(window.window);

            auto& renderer = registry.ctx().get<GLRenderer>();

            auto windowSize = GetProperty<WindowSize>(registry, e)
                .value_or(WindowSize{100, 100});
            auto cameraEntity = GetProperty<CameraReference>(registry, e)
                .value_or(CameraReference{}).entity;

            auto camera = GetProperty<Camera>(registry, cameraEntity).value_or(Camera{});
            auto transform = GetProperty<Transform>(registry, cameraEntity).value_or(Transform{});

            RenderView renderCamera {
                .camera = camera,
                .viewport = windowSize.AsVec2(),
                .viewTransform = Inverse(transform)
            };

            err += renderer.BeginColorPass();
            //err += renderer.DrawIm3d(renderCamera, im3d);

            glfwSwapBuffers(window.window);
        }
    }

    return err;
}

Error okami::GLRendererModule::Destroy(Registry& registry) const {
    Error err;

    PLOG_INFO << "Shutting down GLRenderer...";
    auto& renderer = registry.ctx().get<GLRenderer>();
    err += renderer.Destroy();

    registry.ctx().erase<GLRenderer>();
    return err;
}