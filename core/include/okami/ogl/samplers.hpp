#pragma once

#include <okami/samplers.hpp>
#include <okami/ogl/utils.hpp>

namespace okami {
    struct GLDefaultSamplers {
        GLSampler linearWrap;
        GLSampler linearClamp;
        GLSampler pointWrap;
        GLSampler pointClamp;

        static Expected<GLDefaultSamplers> Create();

        GLSampler const& Select(SamplerType sampler) const;
    };
}