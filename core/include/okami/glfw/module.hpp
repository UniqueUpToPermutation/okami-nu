#pragma once

#include <okami/okami.hpp>
#include <okami/system.hpp>

struct GLFWwindow;

namespace okami {
    struct GlfwWindowInstance {
        GLFWwindow* window = nullptr;
        bool hasFiredWindowClosed = false;
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
        Error PreExecute(entt::registry& registry) const override;
        Error PostExecute(entt::registry& registry) const override;
    };
}