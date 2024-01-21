#pragma once

#include <okami/ogl/utils.hpp>
#include <okami/texture.hpp>

namespace okami {
    GLenum ToGL(texture::Format format);
    GLenum ToBaseFormatGL(texture::Format format);
    
    void DestroyGLTexture(GLuint id);
    struct GLTexture final : public GLuintObject<DestroyGLTexture> {
        texture::Desc desc;

        static Expected<GLTexture> Create(texture::Buffer const& buffer);
        static Expected<GLTexture> Create(texture::Buffer&& buffer);
    };
}