#include <okami/ogl/texture.hpp>

using namespace okami;
using namespace okami::texture;

void okami::DestroyGLTexture(GLuint id) {
    glDeleteTextures(1, &id);
}

GLenum okami::ToGL(texture::Format format) {
    switch (format.channels) {
        case 1:
            if (!format.isLinear) {
                return GL_INVALID_ENUM;
            }

            switch (format.valueType) {
                case ValueType::FLOAT32:
                    return GL_R32F;
                case ValueType::FLOAT16:
                    return GL_R16F;
                case ValueType::INT32:
                    if (format.isNormalized) {
                        return GL_INVALID_ENUM;
                    } else {
                        return GL_R32I;
                    }
                case ValueType::UINT32:
                    return GL_R32UI;
                case ValueType::INT16:
                    if (format.isNormalized) {
                        return GL_R16_SNORM;
                    } else {
                        return GL_R16I;
                    }
                case ValueType::UINT16:
                    return GL_R16UI;
                case ValueType::INT8:
                    if (format.isNormalized) {
                        return GL_R8_SNORM;
                    } else {
                        return GL_R8I;
                    }
                case ValueType::UINT8:
                    return GL_R8UI;
                default:
                    return GL_INVALID_ENUM;
            }
        case 2:
             if (!format.isLinear) {
                return GL_INVALID_ENUM;
            }

            switch (format.valueType) {
                case ValueType::FLOAT32:
                    return GL_RG32F;
                case ValueType::FLOAT16:
                    return GL_RG16F;
                case ValueType::INT32:
                    if (format.isNormalized) {
                        return GL_INVALID_ENUM;
                    } else {
                        return GL_RG32I;
                    }
                case ValueType::UINT32:
                    return GL_RG32UI;
                case ValueType::INT16:
                    if (format.isNormalized) {
                        return GL_RG16_SNORM;
                    } else {
                        return GL_RG16I;
                    }
                case ValueType::INT8:
                    if (format.isNormalized) {
                        return GL_RG8_SNORM;
                    } else {
                        return GL_RG8I;
                    }
                case ValueType::UINT8:
                    return GL_RG8UI;
                default:
                    return GL_INVALID_ENUM;
            }
        case 3:
            if (format.valueType == ValueType::UINT8) {
                if (format.isLinear) {
                    return GL_RGB8UI;
                } else {
                    return GL_SRGB8;
                }
            } else if (format.isLinear) {
                return GL_INVALID_ENUM;
            }

            switch (format.valueType) {
                case ValueType::FLOAT32:
                    return GL_RGB32F;
                case ValueType::FLOAT16:
                    return GL_RGB16F;
                case ValueType::INT32:
                    if (format.isNormalized) {
                        return GL_INVALID_ENUM;
                    } else {
                        return GL_RGB32I;
                    }
                case ValueType::UINT32:
                    return GL_RGB32UI;
                case ValueType::INT16:
                    if (format.isNormalized) {
                        return GL_RGB16_SNORM;
                    } else {
                        return GL_RGB16I;
                    }
                case ValueType::UINT16:
                    return GL_RGB16UI;
                case ValueType::INT8:
                    if (format.isNormalized) {
                        return GL_RGB8_SNORM;
                    } else {
                        return GL_RGB8I;
                    }
                default:
                    return GL_INVALID_ENUM;
            }

        case 4:
            if (format.valueType == ValueType::UINT8) {
                if (format.isLinear) {
                    return GL_RGBA8UI;
                } else {
                    return GL_SRGB8_ALPHA8;
                }
            } else if (format.isLinear) {
                return GL_INVALID_ENUM;
            }

            switch (format.valueType) {
                case ValueType::FLOAT32:
                    return GL_RGBA32F;
                case ValueType::FLOAT16:
                    return GL_RGBA16F;
                case ValueType::INT32:
                    if (format.isNormalized) {
                        return GL_INVALID_ENUM;
                    } else {
                        return GL_RGBA32I;
                    }
                case ValueType::UINT32:
                    return GL_RGBA32UI;
                case ValueType::INT16:
                    if (format.isNormalized) {
                        return GL_RGBA16_SNORM;
                    } else {
                        return GL_RGBA16I;
                    }
                case ValueType::UINT16:
                    return GL_RGBA16UI;
                case ValueType::INT8:
                    if (format.isNormalized) {
                        return GL_RGBA8_SNORM;
                    } else {
                        return GL_RGBA8I;
                    }
                default:
                    return GL_INVALID_ENUM;
            }
        default:
            return GL_INVALID_ENUM;
    }

    return GL_INVALID_ENUM;
}

GLenum okami::ToBaseFormatGL(texture::Format format) {
    switch (format.channels) {
        case 1:
            return GL_RED;
        case 2:
            return GL_RG;
        case 3:
            return GL_RGB;
        case 4:
            return GL_RGBA;
        default:
            return GL_INVALID_ENUM;
    }
    
    return GL_INVALID_ENUM;
}

Expected<GLTexture> GLTexture::Create(texture::Buffer const& buffer) {
    GLTexture tex;
    tex.desc = buffer.GetDesc();

    OKAMI_EXP_RETURN_IF(buffer.desc.type != Dimension::Texture2D, 
        RuntimeError{"Only 2D textures are currently supported!"});
    
    OKAMI_EXP_GL(glGenTextures(1, &*tex));
    OKAMI_EXP_GL(glBindTexture(GL_TEXTURE_2D, *tex));

    // Get all the mips
    for (auto const& subresource : tex.desc.GetSubresourceDescs()) {
        int width = tex.desc.width >> subresource.mip;
        int height = tex.desc.height >> subresource.mip;

        auto internalFormat = ToGL(tex.desc.format);
        auto dataFormat = ToBaseFormatGL(tex.desc.format);

        OKAMI_EXP_GL(glTexImage2D(GL_TEXTURE_2D, subresource.mip, internalFormat, 
            width, height, 0, dataFormat, ToGL(tex.desc.format.valueType), 
            &buffer.data[subresource.srcOffset]));
    }

    return tex;
}

Expected<GLTexture> GLTexture::Create(texture::Buffer&& buffer) {
    return Create(buffer);
}