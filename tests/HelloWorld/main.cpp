#include <okami/okami.hpp>
#include <okami/glfw_module.hpp>
#include <okami/gl_renderer.hpp>

using namespace okami;

int main() {
    Engine en;

    entt::registry reg;

    auto glfw = en.Add<GlfwModule>();
    auto renderer = en.Add<GLRenderer>(*glfw);

    auto window = reg.create();
    glfw->CreateWindow(reg, window);
    renderer->CreateRenderSurface(reg, window);

    en.Initialize(reg);
    while (!CheckSignal<SWindowClosed>(reg, window)) {
        en.Execute(reg);
    }
    en.Destroy(reg);

    return 0;
}