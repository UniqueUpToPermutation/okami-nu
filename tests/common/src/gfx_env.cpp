#include <okami/test/gfx_env.hpp>
#include <okami/ogl/embed.hpp>

#include <plog/Log.h>
#include <plog/Initializers/ConsoleInitializer.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace okami;
using namespace okami::test;

GfxEnvironment* GfxEnvironment::_gSingleton = nullptr;

void GfxEnvironment::ErrorHandler(int error, const char* desc) {
    OKAMI_ERR_SET(_gSingleton->_error, (GlfwError{error, desc}));
}

GfxEnvironment::GfxEnvironment() : _window(nullptr, &glfwDestroyWindow) {
}

GfxEnvironment::GfxEnvironment(GfxEnvironment&& other) : GfxEnvironment() {
    *this = std::move(other);
}

GfxEnvironment& GfxEnvironment::operator=(GfxEnvironment&& other) {
    if (&other == _gSingleton) {
        _gSingleton = this;
    }
    _window = std::move(other._window);
    _error = std::move(other._error);

    return *this;
}

GfxEnvironment::~GfxEnvironment() {
    if (this == _gSingleton) {
        Destroy();
        _gSingleton = nullptr;
    }
}

Expected<GfxEnvironment> GfxEnvironment::Create() {
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);

    GfxEnvironment result;
    result._gSingleton = &result;

    PLOG_INFO << "Initializing GLFW...";
    glfwSetErrorCallback(GfxEnvironment::ErrorHandler);
    
    glfwInit();
    OKAMI_EXP_RETURN(result._error);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto window = glfwCreateWindow(1920, 1080, "Test", nullptr, nullptr);      
    OKAMI_EXP_RETURN(result._error);

    glfwMakeContextCurrent(window);
    result._window.reset(window);

    int version = gladLoadGL();
    OKAMI_EXP_RETURN_IF(!version, RuntimeError{"Failed to load OpenGL!"});

    auto const& fs = GetEmbeddedGLShaderSources();
    std::stringstream ss;
    ss << "Embedded OpenGL Shaders:\n";
    for (auto const& [name, result] : fs) {
        ss << name << "\n";
    }
    PLOG_INFO << ss.str();

    return Expected<GfxEnvironment>(std::move(result));
}

Error GfxEnvironment::Destroy() {
    PLOG_INFO << "Shutting Down GLFW...";

    _error = {};
    _window.reset();

    glfwTerminate();
    OKAMI_ERR_RETURN(_error);
    return {};
}

glm::vec2 GfxEnvironment::GetViewport() const {
    int x, y;
    glfwGetFramebufferSize(_window.get(), &x, &y);
    return glm::vec2(static_cast<float>(x), static_cast<float>(y));
}

bool GfxEnvironment::ShouldClose() const {
    if (_window)
        return glfwWindowShouldClose(_window.get());
    else 
        return true;
}

Error GfxEnvironment::MessagePump() {
    _error = {};
    glfwPollEvents();
    OKAMI_ERR_RETURN(_error);
    return {};
}

Error GfxEnvironment::SwapBuffers() {
    _error = {};
    glfwSwapBuffers(_window.get());
    OKAMI_ERR_RETURN(_error);
    return {};
}