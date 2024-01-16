#pragma once

#include <okami/okami.hpp>

#include <glm/glm.hpp>

struct GLFWwindow;

namespace okami::test {
    class GfxEnvironment {
    private:
        std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> _window;
        static GfxEnvironment* _gSingleton;
        Error _error;

        static void ErrorHandler(int error, const char* desc);

    public:
        GfxEnvironment();
        GfxEnvironment(GfxEnvironment const&) = delete;
        GfxEnvironment(GfxEnvironment&&);
        GfxEnvironment& operator=(GfxEnvironment const&) = delete;
        GfxEnvironment& operator=(GfxEnvironment&&);

        static Expected<GfxEnvironment> Create();
        Error Destroy();

        glm::vec2 GetViewport() const;

        bool ShouldClose() const;
        Error MessagePump();
        Error SwapBuffers();

        ~GfxEnvironment();
    };
}