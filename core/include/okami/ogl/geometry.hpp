#pragma once

#include <okami/geometry.hpp>
#include <okami/ogl/utils.hpp>

namespace okami {
    struct GLGeometry {
        std::vector<GLBuffer> vertexBuffers;
        GLBuffer indexBuffer;
        GLVertexArray vertexArray;
        GeometryDesc desc;

        GLGeometry() = default;
        OKAMI_MOVE_ONLY(GLGeometry);

        static Expected<GLGeometry> Create(Geometry const& geometry);
        static Expected<GLGeometry> Create(Geometry&& geometry);
    };
}