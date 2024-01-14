#include <okami/window.hpp>

using namespace okami;

void okami::RegisterWindowPrototype(std::unordered_map<std::string, Prototype>& proto) {
    proto[std::string{prototypes::Window}].factories.emplace_back([](Registry& reg, entity e) -> Error {
        reg.emplace<SignalSource<SWindowClosed>>(e);
        reg.emplace<WindowSize>(e);
        return {};
    });
}