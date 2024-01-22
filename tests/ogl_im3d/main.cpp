#include <okami/okami.hpp>

#include <okami/ogl/im3d.hpp>
#include <okami/test/gfx_env.hpp>

#include <glad/glad.h>

#include <iostream>
#include <array>

using namespace okami;
using namespace okami::test;

Error TestMain() {
    Error err;
    GfxEnvironment env = OKAMI_ERR_UNWRAP(GfxEnvironment::Create(GfxEnvironmentParams{
        .windowTitle = "Im3d Test"
    }), err);

    {
        GLIm3dRenderer renderer = OKAMI_ERR_UNWRAP(GLIm3dRenderer::Create(), err);

        while (!env.ShouldClose()) {
            err += env.MessagePump();
            OKAMI_ERR_RETURN(err);

            RenderView renderView {
                .camera = Camera{
                    .extents = glm::vec2{2.0f, 2.0f}
                },
                .viewport = env.GetViewport(),
                .viewTransform = {}
            };

            Im3d::Context context;
            Im3d::AppData& appData = context.getAppData();
            appData = Im3dCreateAppData(renderView, env.GetViewport(), env.GetDeltaTime());

            context.begin(Im3d::PrimitiveMode_Triangles);
            context.vertex(Im3d::Vec3{0.5f, 1.0f, 0.5f}, 1.0f, Im3d::Color_Red);
            context.vertex(Im3d::Vec3{1.0f, 0.0f, 0.5f}, 1.0f, Im3d::Color_Green);
            context.vertex(Im3d::Vec3{0.0f, 0.0f, 0.5f}, 1.0f, Im3d::Color_Blue);
            context.end();

            context.begin(Im3d::PrimitiveMode_LineStrip);
            context.vertex(Im3d::Vec3{-0.75f, 0.75f, 0.5f}, 5.0f, Im3d::Color_Red);
            context.vertex(Im3d::Vec3{-0.25f, 0.75f, 0.5f}, 5.0f, Im3d::Color_Brown);
            context.vertex(Im3d::Vec3{-0.25f, 0.25f, 0.5f}, 5.0f, Im3d::Color_Blue);
            context.vertex(Im3d::Vec3{-0.75f, 0.25f, 0.5f}, 5.0f, Im3d::Color_Green);
            context.vertex(Im3d::Vec3{-0.75f, 0.75f, 0.5f}, 5.0f, Im3d::Color_Red);
            context.end();

            context.begin(Im3d::PrimitiveMode_Points);
            context.vertex(Im3d::Vec3{-0.75f, -0.75f, 0.5f}, 50.0f, Im3d::Color_Red);
            context.vertex(Im3d::Vec3{-0.25f, -0.75f, 0.5f}, 50.0f, Im3d::Color_Brown);
            context.vertex(Im3d::Vec3{-0.25f, -0.25f, 0.5f}, 50.0f, Im3d::Color_Blue);
            context.vertex(Im3d::Vec3{-0.75f, -0.25f, 0.5f}, 50.0f, Im3d::Color_Green);
            context.end();

            context.endFrame();

            glEnable(GL_DEPTH_TEST);

            glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	        glViewport(0, 0, (GLsizei)env.GetViewport().x, (GLsizei)env.GetViewport().y);

            renderer.Draw(renderView, context);

            err += env.SwapBuffers();
            OKAMI_ERR_RETURN(err);
        }
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