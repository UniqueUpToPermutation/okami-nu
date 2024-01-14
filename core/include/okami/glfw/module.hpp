#pragma once

#include <okami/okami.hpp>
#include <okami/system.hpp>

#include <glm/glm.hpp>

struct GLFWwindow;

namespace okami {
    struct GlfwWindowInstance {
        GLFWwindow* window = nullptr;
        bool hasFiredWindowClosed = false;
    };

    class GlfwModule final : public Module {
    private:
        static GlfwModule* gSingleton;

        Error _error;

        static void ErrorHandler(int error, const char* desc);

    public:
        GlfwModule();
        ~GlfwModule();

        void RegisterPrototypes(std::unordered_map<std::string, Prototype>& prototypes) const override;
        void CreateWindow(Registry& reg, entity e);

        Error Initialize(Registry& registry) const override;
        Error Destroy(Registry& registry) const override;
        Error PreExecute(Registry& registry) const override;
        Error PostExecute(Registry& registry) const override;
    };
}