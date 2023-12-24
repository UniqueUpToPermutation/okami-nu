#pragma once

#include <okami/okami.hpp>

struct GLFWwindow;

namespace okami {
    struct CGlfwWindowInstance {
        GLFWwindow* window = nullptr;
    };

    struct SWindowClosed {};

    class GlfwModule : public Module {
    private:
        static GlfwModule* gSingleton;

        Error _error;

        static void ErrorHandler(int error, const char* desc);

    public:
        GlfwModule();
        ~GlfwModule();

        void CreateWindow(entt::registry& reg, entity e);

        Error Initialize(entt::registry& registry) const override;
        Error Destroy(entt::registry& registry) const override;
        void PreExecute(entt::registry& registry) const override;
        void PostExecute(entt::registry& registry) const override;
    };
}