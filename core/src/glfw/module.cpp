#include <okami/glfw/module.hpp>
#include <okami/window.hpp>

#include <GLFW/glfw3.h>

#include <plog/Log.h>

using namespace okami;

void okami::GlfwModule::ErrorHandler(int error, const char* desc) {
    OKAMI_ERR_SET(gSingleton->_error, (GlfwError{error, desc}));
}

okami::GlfwModule::GlfwModule() : Module(ModuleDesc{}) {
    if (gSingleton) {
        throw std::runtime_error("Only one instance of GlfwModule allowed!");
    }
    gSingleton = this;
}

okami::GlfwModule::~GlfwModule() {
    gSingleton = nullptr;
}

void okami::GlfwModule::RegisterPrototypes(std::unordered_map<std::string, Prototype>& protoMap) const {
    // Window prototype
    protoMap[std::string{prototypes::Window}].factories.emplace_back(
        [](Registry& reg, entity e) -> Error {
            reg.emplace<GlfwWindowInstance>(e, GlfwWindowInstance{nullptr});
            return {};
        } 
    );
}

Error okami::GlfwModule::Initialize(Registry& registry) const {
    PLOG_INFO << "Initializing GLFW...";
    glfwSetErrorCallback(ErrorHandler);
    
    glfwInit();
    OKAMI_ERR_RETURN(_error);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto view = registry.view<GlfwWindowInstance>();
    for (auto e : view) {
        auto& instance = view.get<GlfwWindowInstance>(e);
        auto window = glfwCreateWindow(1920, 1080, "Test", nullptr, nullptr);      
        OKAMI_ERR_RETURN(_error);
        instance.window = window;
    }

    return {};
}

Error okami::GlfwModule::Destroy(Registry& registry) const {
    PLOG_INFO << "Shutting down GLFW...";
    auto windowView = registry.view<GlfwWindowInstance>();

    for (auto e : windowView) {
        auto& instance = windowView.get<GlfwWindowInstance>(e);
        if (instance.window) {
            glfwDestroyWindow(instance.window);
        }
        instance.window = nullptr;
    }

    glfwTerminate();

    return {};
}

Error okami::GlfwModule::PreExecute(Registry& registry) const {
    glfwPollEvents();

    auto view = registry.template view<GlfwWindowInstance>();
    for (auto e : view) {
        auto& window = view.template get<GlfwWindowInstance>(e);

        // Fire window closed signal if necessary
        if (glfwWindowShouldClose(window.window) && !window.hasFiredWindowClosed) {
            FireSignal<SWindowClosed>(registry, e);
            window.hasFiredWindowClosed = true;
        }

        // Set window size component
        auto windowSize = TryGet<WindowSize>(registry, e);
        if (windowSize) {
            glfwGetFramebufferSize(window.window, &windowSize->x, &windowSize->y);
        }
    }
    return {};
}

Error okami::GlfwModule::PostExecute(Registry& registry) const {
    return {};
}

GlfwModule* okami::GlfwModule::gSingleton = nullptr;