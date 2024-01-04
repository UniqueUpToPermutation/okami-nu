#pragma once

#include <okami/okami.hpp>
#include <okami/embed.hpp>
#include <okami/shader_preprocessor.hpp>
#include <okami/vertex_format.hpp>
#include <glad/glad.h>

namespace okami {
    template <void(Destructor)(GLuint)>
    struct GLuintObject {
        GLuint id = 0u;

        GLuintObject() = default;
        inline GLuintObject(GLuint id) : id(id) {}
        OKAMI_MOVE_ONLY(GLuintObject);

        ~GLuintObject() {
            if (id != 0u) {
                Destructor(id);
            }
            id = 0u;
        }

        inline GLuint& operator*() {
            return id;
        }
        inline GLuint const& operator*() const {
            return id;
        }
    };

    void DestroyGLShader(GLuint id);
    struct GLShader final : public GLuintObject<DestroyGLShader> {
        using GLuintObject::GLuintObject;
    };

    void DestroyGLProgram(GLuint id);
    struct GLProgram final : public GLuintObject<DestroyGLProgram> {
        using GLuintObject::GLuintObject;
    };

    void DestroyGLBuffer(GLuint id);
    struct GLBuffer final : public GLuintObject<DestroyGLProgram> {
        using GLuintObject::GLuintObject;

        static Expected<GLBuffer> Create(BufferData const& buffer);
    };

    void DestroyGLVertexArray(GLuint id);
    struct GLVertexArray final : public GLuintObject<DestroyGLVertexArray> {
        using GLuintObject::GLuintObject;

        static Expected<GLVertexArray> Create(
            
            VertexFormat const& format);
    };

    Expected<GLShader> LoadEmbeddedGLShader(
        std::string_view path, 
        GLenum shaderType,
        ShaderPreprocessorConfig const& config = {});

    ErrorDetails GetErrorGL();

    GLenum ToGL(ValueType valueType);

    #define OKAMI_ERR_GL(statement) \
        statement; \
        { \
            auto details = GetErrorGL(); \
            OKAMI_ERR_RETURN_IF(IsError(details), details); \
        }

    #define OKAMI_EXP_GL(statement) \
        statement; \
        { \
            auto details = GetErrorGL(); \
            OKAMI_EXP_RETURN_IF(IsError(details), details); \
        }
}