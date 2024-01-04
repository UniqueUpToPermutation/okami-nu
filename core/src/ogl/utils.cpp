#include <okami/ogl/utils.hpp>
#include <okami/ogl/embed.hpp>

using namespace okami;

void okami::DestroyGLShader(GLuint id) {
    glDeleteShader(id);
}

void okami::DestroyGLProgram(GLuint id) {
    glDeleteProgram(id);
}

void okami::DestroyGLBuffer(GLuint id) {
    glDeleteBuffers(1, &id);
}

void okami::DestroyGLVertexArray(GLuint id) {
    glDeleteVertexArrays(1, &id);
}

Expected<GLBuffer> okami::GLBuffer::Create(BufferData const& buffer) {
    GLBuffer result;
    OKAMI_EXP_GL(glGenBuffers(1, &*result));
    OKAMI_EXP_GL(glBindBuffer(GL_ARRAY_BUFFER, *result));
    OKAMI_EXP_GL(glBufferData(GL_ARRAY_BUFFER, 
        buffer.size(), buffer.vdata(),  GL_STATIC_READ));
    return result;
}

GLenum okami::ToGL(ValueType valueType) {
    switch (valueType) {
        case ValueType::FLOAT32:
            return GL_FLOAT;
        case ValueType::INT32:
            return GL_INT;
        case ValueType::UINT32:
            return GL_UNSIGNED_INT;
        case ValueType::FLOAT16:
            return GL_HALF_FLOAT;
        case ValueType::INT16:
            return GL_SHORT;
        case ValueType::INT8:
            return GL_BYTE;
        case ValueType::UINT16:
            return GL_UNSIGNED_INT;
        case ValueType::UINT8:
            return GL_UNSIGNED_BYTE;
        case ValueType::NULL_T:
        case ValueType::NUM_TYPES:
        case ValueType::UNDEFINED:
            return 0;
    }
}

ErrorDetails okami::GetErrorGL() {
    auto err = glGetError();
    switch (glGetError()) {
    case GL_NO_ERROR:
        return {};
    case GL_INVALID_ENUM:
        return RuntimeError{"OpenGL: Invalid Enum"};
    case GL_INVALID_VALUE:
        return RuntimeError{"OpenGL: Invalid Value"};
    case GL_INVALID_OPERATION:
        return RuntimeError{"OpenGL: Invalid Operation"};
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return RuntimeError{"OpenGL: Invalid Framebuffer Operation"};
    case GL_OUT_OF_MEMORY:
        return RuntimeError{"OpenGL: Out of Memory"};
    case GL_STACK_UNDERFLOW:
        return RuntimeError{"OpenGL: Stack Underflow"};
    case GL_STACK_OVERFLOW:
        return RuntimeError{"OpenGL: Stack Overflow"};
    default:
        return RuntimeError{"OpenGL: Unknown Error"};
    }
}

Expected<GLShader> okami::LoadEmbeddedGLShader(
    std::string_view path, 
    GLenum shaderType,
    ShaderPreprocessorConfig const& config) {
    Error err;

    auto const& fs = GetEmbeddedGLShaderSources();

    // Create shader
    GLShader shader = OKAMI_EXP_GL(glCreateShader(shaderType));

    // Get shader source from filesystem
    auto shaderSrc = OKAMI_EXP_UNWRAP(
        ShaderPreprocessor::Load(path, fs, {}, nullptr, true), err);
    const char* pData[] = { shaderSrc.content.data() };
    GLint pLen[] = { static_cast<GLint>(shaderSrc.content.size()) };
    OKAMI_EXP_GL(glShaderSource(*shader, 1, pData, pLen));

    // Compile
    OKAMI_EXP_GL(glCompileShader(*shader));

    return shader;
}

