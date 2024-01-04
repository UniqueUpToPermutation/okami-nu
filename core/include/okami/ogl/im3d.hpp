#pragma once

#include <okami/okami.hpp>
#include <glad/glad.h>

#include <okami/ogl/utils.hpp>

#include <im3d.h>

namespace okami {
    struct GLIm3dRenderer {
    private:
        GLVertexArray vertexArray;
        GLBuffer vertexBuffer;

        struct Shader {
            GLProgram shader;
            GLint uViewport;
            GLint uViewProjMatrix;
        };

        Shader points;
        Shader lines;
        Shader triangles;

    public:
        static Expected<GLIm3dRenderer> Create();

        Error Draw(Im3d::Context& context, 
            int viewportWidth, int viewportHeight) const;
    };
}