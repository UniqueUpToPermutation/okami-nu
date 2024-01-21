#pragma once

#include <okami/ogl/utils.hpp>
#include <okami/ogl/geometry.hpp>
#include <okami/ogl/samplers.hpp>
#include <okami/ogl/material.hpp>
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

    struct GLTexturedUniformBlock {
        GLint uTextureSampler;
        GLint uColor;

        static Expected<GLTexturedUniformBlock> Create(GLProgram const& program);

        void Set(GLTexturedMaterial const& mat, 
            GLTexture const& defaultTex,
            GLDefaultSamplers const& samplers) const;
    };

    struct GLStaticMeshRenderCall {
        GLGeometry const& geometry;
        std::optional<GLTexturedMaterial> material;
        Transform transform;
    };

    class GLStaticMeshRenderer {
    private:
        GLProgram _renderProgram;
        GLCameraUniformBlock _cameraUniforms;
        GLWorldUniformBlock _worldUniforms;
        GLTexturedUniformBlock _texturedUniforms;
        GLDefaultSamplers _samplers;
        GLTexture _defaultTexture;

    public:
        static Expected<GLStaticMeshRenderer> Create();

        inline static VertexFormatInfo GetVertexFormat() {
            return VertexFormatInfo::PositionUV();
        }
        Error Draw(RenderView const& camera, std::span<GLStaticMeshRenderCall const> meshes) const;
    };
}