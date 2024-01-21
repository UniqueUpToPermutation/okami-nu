#pragma once

#include <okami/ogl/texture.hpp>

namespace okami {
    struct GLTexturedMaterial {
        glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
        GLTexture const* texture = nullptr;
        SamplerType textureSampler = SamplerType::LINEAR_WRAP;
    };
}