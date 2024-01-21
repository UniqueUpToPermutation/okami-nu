#pragma once

#include <okami/okami.hpp>

#include <glm/glm.hpp>

struct GLFWwindow;

namespace okami::test {
    struct GfxEnvironmentParams {
        std::string windowTitle = "Graphics Environment";
    };

    class GfxEnvironment {
    private:
        std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> _window;
        static GfxEnvironment* _gSingleton;
        Error _error;
        bool _isTimerInitialized = false;
        double _lastTime = 0.0;
        double _deltaTime = 0.0;
        GfxEnvironmentParams _params;

        static void ErrorHandler(int error, const char* desc);

    public:
        GfxEnvironment(GfxEnvironmentParams params = GfxEnvironmentParams{});
        GfxEnvironment(GfxEnvironment const&) = delete;
        GfxEnvironment(GfxEnvironment&&);
        GfxEnvironment& operator=(GfxEnvironment const&) = delete;
        GfxEnvironment& operator=(GfxEnvironment&&);

        static Expected<GfxEnvironment> Create(GfxEnvironmentParams params = GfxEnvironmentParams{});
        Error Destroy();

        glm::vec2 GetViewport() const;

        bool ShouldClose() const;
        Error MessagePump();
        Error SwapBuffers();

        double GetTime() const;
        double GetDeltaTime() const;

        ~GfxEnvironment();
    };
}