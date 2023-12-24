#pragma once

#include <okami/okami.hpp>
#include <okami/glfw_module.hpp>

namespace okami {
    class GLRenderer : public Module {
    private:

    public:
        GLRenderer(GlfwModule const& module);

        Error CreateRenderSurface(entt::registry& registry, entity e);

        Error Initialize(entt::registry& registry) const override;
        void PreExecute(entt::registry& registry) const override;
        void PostExecute(entt::registry& registry) const override;
        Error Destroy(entt::registry& registry) const override;
    };
}