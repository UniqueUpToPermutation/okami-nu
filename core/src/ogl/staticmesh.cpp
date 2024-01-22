#include <okami/ogl/staticmesh.hpp>

#include <plog/Log.h>

using namespace okami;

Expected<GLWorldUniformBlock> GLWorldUniformBlock::Create(GLProgram const& program) {
    GLWorldUniformBlock block;
    block.uWorld = UnwrapAndWarn(program.GetUniformLocation("uWorld"), -1);
    block.uWorldInvTrans = UnwrapAndWarn(program.GetUniformLocation("uWorldInvTrans"), -1);
    return block;
}

void GLWorldUniformBlock::Set(glm::mat4 const& world) const {
    auto invTrans = glm::transpose(glm::inverse(world));
    glUniformMatrix4fv(uWorld, 1, false, &world[0][0]);
    glUniformMatrix4fv(uWorldInvTrans, 1, false, &invTrans[0][0]);
}

Expected<GLCameraUniformBlock> GLCameraUniformBlock::Create(GLProgram const& program) {
    GLCameraUniformBlock block;
    block.uView = UnwrapAndWarn(program.GetUniformLocation("uView"), -1);
    block.uProj = UnwrapAndWarn(program.GetUniformLocation("uProj"), -1);
    block.uViewProj = UnwrapAndWarn(program.GetUniformLocation("uViewProj"), -1);
    return block;
}

void GLCameraUniformBlock::Set(glm::mat4 const& view, glm::mat4 const& proj) const {
    auto viewProj = proj * view;
    glUniformMatrix4fv(uView, 1, false, &view[0][0]);
    glUniformMatrix4fv(uProj, 1, false, &proj[0][0]);
    glUniformMatrix4fv(uViewProj, 1, false, &viewProj[0][0]);
}

Expected<GLTexturedUniformBlock> GLTexturedUniformBlock::Create(GLProgram const& program) {
    GLTexturedUniformBlock result;
    result.uTextureSampler = UnwrapAndWarn(program.GetUniformLocation("uTextureSampler"), -1);
    result.uColor = UnwrapAndWarn(program.GetUniformLocation("uColor"), -1);
    return result;
}

void GLTexturedUniformBlock::Set(
    GLTexturedMaterial const& mat, 
    GLTexture const& defaultTex,
    GLDefaultSamplers const& samplers) const {
    glActiveTexture(GL_TEXTURE0);
    if (mat.texture) {
        glBindTexture(GL_TEXTURE_2D, **mat.texture);
        glBindSampler(0, *samplers.Select(mat.textureSampler));
    } else {
        glBindTexture(GL_TEXTURE_2D, *defaultTex);
        glBindSampler(0, *samplers.Select(SamplerType::POINT_WRAP));
    }
    glUniform1i(uTextureSampler, 0);
    glUniform4fv(uColor, 1, &mat.color[0]);
}

Expected<GLStaticMeshRenderer> GLStaticMeshRenderer::Create() {
    Error err;
    GLStaticMeshRenderer result;

    auto texture = texture::prefabs::CheckerBoard(16, 16, 8, 8);
    result._defaultTexture = OKAMI_EXP_UNWRAP(GLTexture::Create(std::move(texture)), err);

    auto vs = OKAMI_EXP_UNWRAP(LoadEmbeddedGLShader("staticmesh.vs", GL_VERTEX_SHADER, {}), err);
    auto fs = OKAMI_EXP_UNWRAP(LoadEmbeddedGLShader("textured.fs", GL_FRAGMENT_SHADER, {}), err);

    result._renderProgram = OKAMI_EXP_UNWRAP(CreateProgram(std::array{*vs, *fs}), err);
    result._cameraUniforms = OKAMI_EXP_UNWRAP(GLCameraUniformBlock::Create(result._renderProgram), err);
    result._worldUniforms = OKAMI_EXP_UNWRAP(GLWorldUniformBlock::Create(result._renderProgram), err);
    result._samplers = OKAMI_EXP_UNWRAP(GLDefaultSamplers::Create(), err);
    result._texturedUniforms = OKAMI_EXP_UNWRAP(GLTexturedUniformBlock::Create(result._renderProgram), err);

    return result;
}

Error GLStaticMeshRenderer::Draw(RenderView const& camera, std::span<GLStaticMeshRenderCall const> meshes) const {
    OKAMI_ERR_GL(glUseProgram(*_renderProgram));

    // Set the camera view and projection transforms
    _cameraUniforms.Set(camera.GetViewMatrix(), camera.GetProjMatrix());
    
    for (auto mesh : meshes) {
        if (mesh.geometry.desc.layout.formatTag == VertexFormat::PositionUV) {
            // Set the world transform
            _worldUniforms.Set(mesh.transform.ToMatrix4x4());

            // Bind the material
            auto material = mesh.material.value_or(GLTexturedMaterial{});
            _texturedUniforms.Set(material, _defaultTexture, _samplers);

            // Bind the mesh
            OKAMI_ERR_GL(glBindVertexArray(*mesh.geometry.vertexArray));

            GLenum topology = ToGL(mesh.geometry.desc.topology);

            if (!mesh.geometry.desc.isIndexed) {
                OKAMI_ERR_GL(glDrawArrays(topology, 0, mesh.geometry.desc.attribs.numVertices));
            } else {
                OKAMI_ERR_GL(glDrawElements(topology, mesh.geometry.desc.indexedAttribs.numIndices,
                    ToGL(mesh.geometry.desc.indexedAttribs.indexType), nullptr));
            }
        } else {
            PLOG_WARNING << "Vertex format is not PositionUV!";
        }
    }

    return {};
}