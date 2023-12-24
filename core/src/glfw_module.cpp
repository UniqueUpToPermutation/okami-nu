#include <okami/glfw_module.hpp>
#include <GLFW/glfw3.h>

using namespace okami;

void okami::GlfwModule::ErrorHandler(int error, const char* desc) {
    OKAMI_ERR_SET(gSingleton->_error, (ErrorGlfw{error, desc}));
}

okami::GlfwModule::GlfwModule() {
    if (gSingleton) {
        throw std::runtime_error("Only one instance of GlfwModule allowed!");
    }
    gSingleton = this;
}

okami::GlfwModule::~GlfwModule() {
    gSingleton = nullptr;
}

void okami::GlfwModule::CreateWindow(entt::registry& reg, entity e) {
    reg.emplace<CGlfwWindowInstance>(e, CGlfwWindowInstance{nullptr});
    reg.emplace<SignalSource<SWindowClosed>>(e);
}

Error okami::GlfwModule::Initialize(entt::registry& registry) const {
    glfwSetErrorCallback(ErrorHandler);
    
    glfwInit();
    OKAMI_ERR_RETURN(_error);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto view = registry.view<CGlfwWindowInstance>();
    for (auto e : view) {
        auto& instance = view.get<CGlfwWindowInstance>(e);
        auto window = glfwCreateWindow(1920, 1080, "Test", nullptr, nullptr);      
        OKAMI_ERR_RETURN(_error);
        instance.window = window;
    }

    return {};
}

Error okami::GlfwModule::Destroy(entt::registry& registry) const {
    auto windowView = registry.view<CGlfwWindowInstance>();

    for (auto e : windowView) {
        auto& instance = windowView.get<CGlfwWindowInstance>(e);
        if (instance.window) {
            glfwDestroyWindow(instance.window);
        }
        instance.window = nullptr;
    }

    glfwTerminate();

    return {};
}

void okami::GlfwModule::PreExecute(entt::registry& registry) const {
    glfwPollEvents();

    auto view = registry.view<CGlfwWindowInstance, SignalSource<SWindowClosed>>();
    for (auto e : view) {
        auto& instance = view.get<CGlfwWindowInstance>(e);

        if (instance.window) {
            auto shouldClose = glfwWindowShouldClose(instance.window);
            if (shouldClose) {
                FireSignal<SWindowClosed>(view, e);
            }
        }
    }
}

void okami::GlfwModule::PostExecute(entt::registry& registry) const {

}

GlfwModule* okami::GlfwModule::gSingleton = nullptr;