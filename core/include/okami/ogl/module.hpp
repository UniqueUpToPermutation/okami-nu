#pragma once

#include <okami/okami.hpp>
#include <okami/glfw/module.hpp>

namespace okami {
    class GLRendererModule : public Module {
    public:
        GLRendererModule(GlfwModule const& module);

        Error CreateRenderSurface(entt::registry& registry, entity e);

        Error Initialize(entt::registry& registry) const override;
        Error PreExecute(entt::registry& registry) const override;
        Error PostExecute(entt::registry& registry) const override;
        Error Destroy(entt::registry& registry) const override;
    };
}