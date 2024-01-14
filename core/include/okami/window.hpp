#pragma once

#include <okami/okami.hpp>
#include <glm/glm.hpp>

namespace okami {
    namespace prototypes {
        constexpr std::string_view Window = "protoWindow";
    }

    struct SWindowClosed {};

    struct WindowSize {
        int x;
        int y;

        inline glm::vec2 AsVec2() const {
            return glm::vec2(static_cast<float>(x), static_cast<float>(y));
        }
    };

    void RegisterWindowPrototype(std::unordered_map<std::string, Prototype>& prototype);
}