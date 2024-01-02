#pragma once

#include <okami/okami.hpp>
#include <glad/glad.h>

#include <okami/ogl/im3d.hpp>

namespace okami {
    class GLRenderer {
    private:
        Im3dRenderer im3d;

    public:
        Error Initialize();
        Error Draw();
        Error Destroy();
    };
}