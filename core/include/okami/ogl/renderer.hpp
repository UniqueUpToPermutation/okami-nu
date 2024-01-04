#pragma once

#include <okami/okami.hpp>
#include <okami/camera.hpp>

#include <glad/glad.h>

#include <okami/ogl/im3d.hpp>

namespace okami {
    class GLRenderer {
    private:
        GLIm3dRenderer im3d;

    public:
        GLRenderer() = default;
        OKAMI_MOVE_ONLY(GLRenderer);

        Error Initialize();
        Error Draw(CameraRenderData const& camera, Im3d::Context& context);
        Error Destroy();
    };
}