#include <okami/ogl/samplers.hpp>

using namespace okami;

Expected<GLDefaultSamplers> GLDefaultSamplers::Create() {
    GLDefaultSamplers collection;

    glGenSamplers(1, &*collection.linearWrap);
    glSamplerParameteri(*collection.linearWrap, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glSamplerParameteri(*collection.linearWrap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(*collection.linearWrap, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(*collection.linearWrap, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(*collection.linearWrap, GL_TEXTURE_WRAP_R, GL_REPEAT);

    glGenSamplers(1, &*collection.linearClamp);
    glSamplerParameteri(*collection.linearClamp, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glSamplerParameteri(*collection.linearClamp, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(*collection.linearClamp, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glSamplerParameteri(*collection.linearClamp, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glSamplerParameteri(*collection.linearClamp, GL_TEXTURE_WRAP_R, GL_CLAMP);

    glGenSamplers(1, &*collection.pointWrap);
    glSamplerParameteri(*collection.pointWrap, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(*collection.pointWrap, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(*collection.pointWrap, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(*collection.pointWrap, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glSamplerParameteri(*collection.pointWrap, GL_TEXTURE_WRAP_R, GL_REPEAT);

    glGenSamplers(1, &*collection.pointClamp);
    glSamplerParameteri(*collection.pointClamp, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameteri(*collection.pointClamp, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(*collection.pointClamp, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glSamplerParameteri(*collection.pointClamp, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glSamplerParameteri(*collection.pointClamp, GL_TEXTURE_WRAP_R, GL_CLAMP);

    OKAMI_EXP_GL();

    return collection;
}

GLSampler const& GLDefaultSamplers::Select(SamplerType sampler) const {
    switch (sampler) {
        case SamplerType::LINEAR_CLAMP:
            return linearClamp;
        case SamplerType::LINEAR_WRAP:
            return linearWrap;
        case SamplerType::POINT_CLAMP:
            return pointClamp;
        case SamplerType::POINT_WRAP:
            return pointWrap;
        default:
            return pointWrap;
    }
}