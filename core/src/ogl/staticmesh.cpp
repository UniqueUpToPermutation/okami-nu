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

Expected<GLStaticMeshRenderer> GLStaticMeshRenderer::Create() {
    Error err;
    GLStaticMeshRenderer result;

    auto vs = OKAMI_EXP_UNWRAP(LoadEmbeddedGLShader("staticmesh.vs", GL_VERTEX_SHADER, {}), err);
    auto fs = OKAMI_EXP_UNWRAP(LoadEmbeddedGLShader("flatcolor.fs", GL_FRAGMENT_SHADER, {}), err);

    result._renderProgram = OKAMI_EXP_UNWRAP(CreateProgram(std::array{*vs, *fs}), err);
    result._cameraUniforms = OKAMI_EXP_UNWRAP(GLCameraUniformBlock::Create(result._renderProgram), err);
    result._worldUniforms = OKAMI_EXP_UNWRAP(GLWorldUniformBlock::Create(result._renderProgram), err);

    return result;
}

Error GLStaticMeshRenderer::Draw(RenderView const& camera, std::span<GLStaticMeshRenderCall const> meshes) const {
    OKAMI_ERR_GL(glDisable(GL_DEPTH_TEST));
    OKAMI_ERR_GL(glDisable(GL_CULL_FACE));
    OKAMI_ERR_GL(glDisable(GL_SCISSOR_TEST));
    OKAMI_ERR_GL(glDisable(GL_STENCIL_TEST));

    OKAMI_ERR_GL(glUseProgram(*_renderProgram));
    OKAMI_ERR_GL(glViewport(0, 0, camera.viewport.x, camera.viewport.y));

    // Set the camera view and projection transforms
    _cameraUniforms.Set(camera.GetViewMatrix(), camera.GetProjMatrix());
    
    for (auto mesh : meshes) {
        if (mesh.geometry.desc.layout.formatTag == VertexFormat::PositionUV) {
            // Set the world transform
            _worldUniforms.Set(mesh.transform.ToMatrix4x4());

            // Render the mesh
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