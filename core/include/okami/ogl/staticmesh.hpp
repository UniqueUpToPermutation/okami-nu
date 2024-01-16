#pragma once

#include <okami/ogl/utils.hpp>
#include <okami/ogl/geometry.hpp>
#include <okami/transform.hpp>
#include <okami/camera.hpp>

#include <span>

namespace okami {
    struct GLCameraUniformBlock {
        GLint uView;
        GLint uProj;
        GLint uViewProj;

        static Expected<GLCameraUniformBlock> Create(GLProgram const& program);

        void Set(glm::mat4 const& view, glm::mat4 const& proj) const;
    };

    struct GLWorldUniformBlock {
        GLint uWorld;
        GLint uWorldInvTrans;

        static Expected<GLWorldUniformBlock> Create(GLProgram const& program);

        void Set(glm::mat4 const& world) const;
    };

    struct GLStaticMeshMaterial {
    };

    struct GLStaticMeshRenderCall {
        GLGeometry const& geometry;
        GLStaticMeshMaterial const& material;
        Transform transform;
    };

    class GLStaticMeshRenderer {
    private:
        GLProgram _renderProgram;
        GLCameraUniformBlock _cameraUniforms;
        GLWorldUniformBlock _worldUniforms;

    public:
        static Expected<GLStaticMeshRenderer> Create();

        static VertexFormat GetVertexFormat();
        Error Draw(RenderView const& camera, std::span<GLStaticMeshRenderCall const> meshes) const;
    };
}