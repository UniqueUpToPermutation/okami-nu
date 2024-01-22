#pragma once

#include <okami/okami.hpp>
#include <glad/glad.h>

#include <okami/ogl/utils.hpp>
#include <okami/camera.hpp>

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

        Error Draw(RenderView const& camera, Im3d::Context& context) const;
    };

    Im3d::Vec2 Im3dConv(glm::vec2 a);
    Im3d::Vec3 Im3dConv(glm::vec3 a);
    Im3d::Vec4 Im3dConv(glm::vec4 a);

    glm::vec2 Im3dConv(Im3d::Vec2 a);
    glm::vec3 Im3dConv(Im3d::Vec3 a);
    glm::vec4 Im3dConv(Im3d::Vec4 a);

    Im3d::AppData Im3dCreateAppData(RenderView const& camera, glm::vec2 viewport, double deltaTime);
}