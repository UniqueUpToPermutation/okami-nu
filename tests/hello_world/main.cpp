#include <okami/okami.hpp>
#include <okami/window.hpp>

#include <iostream>

using namespace okami;

Error EngineMain() {
    Engine en;
    Error err;
    Registry reg;

    auto window = reg.create();
    en.Spawn(reg, window, prototypes::Window);

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

    return err.IsError();
}