#pragma once

#include <okami/okami.hpp>
#include <okami/glfw/module.hpp>

namespace okami {
    struct CameraReference {
        entity entity = null;
    };

    class GLRendererModule : public Module {
    public:
        GLRendererModule(GlfwModule const& module);

        void RegisterPrototypes(std::unordered_map<std::string, Prototype>& prototypes) const override;

        Error Initialize(Registry& registry) const override;
        Error PreExecute(Registry& registry) const override;
        Error PostExecute(Registry& registry) const override;
        Error Destroy(Registry& registry) const override;
    };
}