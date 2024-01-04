#include <okami/okami.hpp>
#include <okami/log.hpp>

#include <okami/glfw/module.hpp>
#include <okami/ogl/module.hpp>

using namespace okami;

std::string_view okami::Module::GetName() const {
    return GetDesc().name;
}

ModuleDesc const& okami::Module::GetDesc() const {
    return _desc;
}

EngineDesc const& okami::Engine::GetDesc() const {
    return _desc;
}

struct okami::Engine::Impl {
    GlfwModule* glfw = nullptr;
    GLRendererModule* renderer = nullptr;
};

okami::Engine::Engine() {
    log::Init();
    PLOG_INFO << "Okami Engine v" << kMajorVersion << "." << kMinorVersion;

    auto glfw = Add<GlfwModule>();
    Add<GLRendererModule>(*glfw);
}

Error okami::Engine::Initialize(entt::registry& registry) const {
    Error err;

    PLOG_INFO << "Initializing engine...";
    for (auto const& module : _desc.modules) {
        err += module->Initialize(registry);
    }

    Log(err);
    return err;
}

Error okami::Engine::Destroy(entt::registry& registry) const {
    Error err;

    PLOG_INFO << "Shutting down engine...";
    for (auto it = _desc.modules.rbegin(); it != _desc.modules.rend(); ++it) {
        err += (*it)->Destroy(registry);
    }

    Log(err);
    return err;
}

Error okami::Engine::Execute(entt::registry& registry) const {   
    for (auto const& module : _desc.modules) {
        OKAMI_ERR_RETURN_IF_FAIL(module->PreExecute(registry));
    }

    for (auto const& module : _desc.modules) {
        OKAMI_ERR_RETURN_IF_FAIL(module->PostExecute(registry));
    }

    return {};
}