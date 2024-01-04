#include <okami/ogl/geometry.hpp>

using namespace okami;

Expected<GLGeometry> GLGeometry::Create(Geometry const& geometry) {
    Error err;

    auto desc = geometry.GetDesc();

    std::vector<GLBuffer> vertexBuffers;
    for (auto const& buffer : geometry.GetVertexBuffers()) {
        auto vertBufferGL = OKAMI_EXP_UNWRAP(GLBuffer::Create(buffer), err);
        vertexBuffers.emplace_back(std::move(vertBufferGL));
    }

    GLBuffer indexBuffer;
    if (desc.isIndexed) {
        auto indexBufferGL = OKAMI_EXP_UNWRAP(
            GLBuffer::Create(geometry.GetIndexBuffer()), err);
    }

    GLVertexArray vao;
    OKAMI_EXP_GL(glGenVertexArrays(1, &*vao));
    OKAMI_EXP_GL(glBindVertexArray(*vao));
    
    for (auto const& layoutElement : desc.layout.elements) {
        OKAMI_EXP_GL(glEnableVertexAttribArray(layoutElement.bufferSlot));
        glVertexAttribPointer(layoutElement.bufferSlot, 
            layoutElement.numComponents,
            ToGL(layoutElement.valueType),
            layoutElement.isNormalized,
            layoutElement.stride,
            (GLvoid*)layoutElement.relativeOffset);
    }

    return {};
}