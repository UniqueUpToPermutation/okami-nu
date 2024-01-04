#include <okami/okami.hpp>
#include <okami/glfw/module.hpp>
#include <okami/ogl/module.hpp>

#include <iostream>

using namespace okami;

Error EngineMain() {
    Engine en;
    entt::registry reg;
    Error err;

    auto glfw = en.Get<GlfwModule>();
    auto renderer = en.Get<GLRendererModule>();
    OKAMI_ERR_RETURN(glfw);
    OKAMI_ERR_RETURN(renderer);

    auto window = reg.create();
    
    glfw->CreateWindow(reg, window);
    err += renderer->CreateRenderSurface(reg, window);

    err += en.Initialize(reg);
    while (err.IsOk() && !CheckSignal<SWindowClosed>(reg, window)) {
        err += en.Execute(reg);
    }
    err += en.Destroy(reg);

    return err;
}

int main() {
    auto err = EngineMain();

    if (err.IsError()) {
        std::cout << err.ToString() << std::endl;
    }

    return 0;
}