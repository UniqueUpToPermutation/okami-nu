#include <okami/okami.hpp>

#include <okami/glfw/module.hpp>
#include <okami/ogl/module.hpp>

#include <okami/window.hpp>

#include <plog/Log.h>

using namespace okami;

void okami::Module::RegisterPrototypes(std::unordered_map<std::string, Prototype>& prototype) const {
}

std::string_view okami::Module::GetName() const {
    return GetDesc().name;
}

ModuleDesc const& okami::Module::GetDesc() const {
    return _desc;
}

EngineDesc const& okami::Engine::GetDesc() const {
    return _desc;
}

void okami::Engine::RegisterDefaultPrototypes() {
    RegisterWindowPrototype(_desc.prototypes);
}

void okami::Engine::CreateDefaultModules() {
    auto glfw = Add<GlfwModule>();
    Add<GLRendererModule>(*glfw);
}

okami::Engine::Engine() {
    log::Init();
    PLOG_INFO << "Okami Engine v" << kMajorVersion << "." << kMinorVersion;

    RegisterDefaultPrototypes();
    CreateDefaultModules();
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

ExpectedRef<Prototype const> okami::Engine::GetPrototype(std::string const& prototype) const {
    auto it = _desc.prototypes.find(prototype);
    OKAMI_EXP_RETURN_IF(it == _desc.prototypes.end(), RuntimeError{"Prototype does not exist!"});
    return std::cref(it->second);
}

Error okami::Engine::Spawn(entt::registry& registry, entt::entity e, std::string const& prototype) const {
    auto proto = GetPrototype(prototype);
    OKAMI_ERR_RETURN(proto);
    return proto->Spawn(registry, e);
}

Error okami::Prototype::Spawn(entt::registry& reg, entt::entity e) const {
    for (auto const& fac : factories) {
        Error err = fac(reg, e);
        OKAMI_ERR_RETURN(err);    
    }
    return {};
}