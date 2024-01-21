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
        .windowTitle = "Texture Test"
    }), err);

    {
        GLStaticMeshRenderer meshRenderer = OKAMI_ERR_UNWRAP(GLStaticMeshRenderer::Create(), err);

        auto layout = meshRenderer.GetVertexFormat();
        Geometry geometry = geometry::prefabs::MaterialBall(layout);
        GLGeometry gpuGeometry = OKAMI_ERR_UNWRAP(GLGeometry::Create(geometry), err);

        auto cpuTexture = OKAMI_ERR_UNWRAP(
            texture::Load("tests/common/assets/texture.png", TextureLoadParams{
                .isSRGB = true,
                .generateMips = true
        }), err);
        auto gpuTexture = OKAMI_ERR_UNWRAP(GLTexture::Create(std::move(cpuTexture)), err);

        GLTexturedMaterial material{
            .texture = &gpuTexture,
        };
        auto calls = std::array{
            GLStaticMeshRenderCall{
                .geometry = gpuGeometry,
                .material = material,
            },
        };

        while (!env.ShouldClose()) {
            err += env.MessagePump();
            OKAMI_ERR_RETURN(err);

            glEnable(GL_DEPTH_TEST);

            glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::vec3 cameraEye{1.0f, 1.0f, 1.0f};
            glm::vec3 cameraTarget{0.0f, 0.0f, 0.0f};
            glm::vec3 cameraUp{0.0f, 1.0f, 0.0f};

            Camera camera;
            camera.near = 1.0f;
            camera.far = 10.0f;
            camera.variant = CameraVariantPerspective{.fieldOfView = glm::pi<float>() / 4.0f};

            glm::quat cameraRotate = glm::angleAxis<float>(env.GetTime(), cameraUp);
            cameraEye = glm::rotate(cameraRotate, cameraEye);
            cameraEye *= 5.0f;

            RenderView renderView {
                .camera = camera,
                .viewport = env.GetViewport(),
                .viewTransform = (Transform::Translate(0.0f, 1.0f, 0.0f) *
                    Transform::LookAt(cameraEye, cameraTarget, cameraUp)).Inverse()
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