#include <okami/okami.hpp>

#include <okami/ogl/staticmesh.hpp>

#include <okami/test/gfx_env.hpp>

#include <glad/glad.h>

#include <iostream>
#include <array>

using namespace okami;
using namespace okami::test;

Error TestMain() {
    Error err;
    GfxEnvironment env = OKAMI_ERR_UNWRAP(GfxEnvironment::Create(GfxEnvironmentParams{
        .windowTitle = "Static Mesh Test"
    }), err);

    {
        GLStaticMeshRenderer meshRenderer = OKAMI_ERR_UNWRAP(GLStaticMeshRenderer::Create(), err);

        auto layout = meshRenderer.GetVertexFormat();
        geometry::Data<> data {
            .positions = {
                glm::vec3(0.0f, 0.5f, 0.0f),
                glm::vec3(0.5f, -0.5f, 0.0f),
                glm::vec3(-0.5f, -0.5f, 0.0f)
            },
            .uvs = {
                {
                    glm::vec2(0.5f, 0.0f),
                    glm::vec2(1.0f, 1.0f),
                    glm::vec2(0.0f, 1.0f)
                }
            },
        };

        Geometry geometry = Geometry::Pack(layout, data);
        GLGeometry gpuGeometry = OKAMI_ERR_UNWRAP(GLGeometry::Create(geometry), err);

        GLTexturedMaterial material;
        auto calls = std::array{
            GLStaticMeshRenderCall{
                .geometry = gpuGeometry,
                .material = material,
                .transform = Transform::Translate(0.5f, 0.5f)
            },
            GLStaticMeshRenderCall{
                .geometry = gpuGeometry,
                .material = material,
                .transform = Transform::Translate(-0.5f, 0.5f) * 
                    Transform::Rotate2D(glm::pi<float>())
            },
            GLStaticMeshRenderCall{
                .geometry = gpuGeometry,
                .material = material,
                .transform = Transform::Translate(-0.5f, -0.5f) * 
                    Transform::Scale(0.5f)
            },
            GLStaticMeshRenderCall{
                .geometry = gpuGeometry,
                .material = material,
                .transform = Transform::Translate(0.5f, -0.5f) * 
                    Transform::Rotate2D(glm::pi<float>() / 2.f) * 
                    Transform::Scale(0.5f)
            },
        };

        while (!env.ShouldClose()) {
            err += env.MessagePump();
            OKAMI_ERR_RETURN(err);

            glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	        glViewport(0, 0, (GLsizei)env.GetViewport().x, (GLsizei)env.GetViewport().y);

            RenderView renderView {
                .camera = {},
                .viewport = env.GetViewport(),
                .viewTransform = {}
            };

            err += meshRenderer.Draw(renderView, calls);
            OKAMI_ERR_RETURN(err);

            err += env.SwapBuffers();
            OKAMI_ERR_RETURN(err);
        }

        meshRenderer = {};
        gpuGeometry = {};
    }

    return {};
}

int main() {
    auto err = TestMain();

    if (err.IsError()) {
        std::cout << err.ToString() << std::endl;
    }

    return err.IsError();
}