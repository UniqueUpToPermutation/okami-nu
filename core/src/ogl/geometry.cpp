#include <okami/ogl/geometry.hpp>

#include <plog/Log.h>

using namespace okami;

Expected<GLGeometry> GLGeometry::Create(Geometry const& geometry) {
    GLGeometry geo;
    geo.desc = geometry.GetDesc();
    auto err = geo.desc.layout.AutoLayout();
    OKAMI_EXP_RETURN(err);

    for (auto const& buffer : geometry.GetVertexBuffers()) {
        auto vertBufferGL = OKAMI_EXP_UNWRAP(GLBuffer::Create(buffer), err);
        geo.vertexBuffers.emplace_back(std::move(vertBufferGL));
    }

    if (geo.desc.isIndexed) {
        geo.indexBuffer = OKAMI_EXP_UNWRAP(
            GLBuffer::Create(geometry.GetIndexBuffer()), err);
    }

    OKAMI_EXP_GL(glGenVertexArrays(1, &*geo.vertexArray));
    OKAMI_EXP_GL(glBindVertexArray(*geo.vertexArray));
    
    for (auto const& layoutElement : geo.desc.layout.elements) {
        OKAMI_EXP_RETURN_IF(layoutElement.bufferSlot >= geo.vertexBuffers.size(),
            RuntimeError{"Layout element buffer slot is outside of acceptible range!"})
        OKAMI_EXP_GL(glBindBuffer(GL_ARRAY_BUFFER, *geo.vertexBuffers[layoutElement.bufferSlot]));
        OKAMI_EXP_GL(glEnableVertexAttribArray(layoutElement.inputIndex));
        OKAMI_EXP_GL(glVertexAttribPointer(layoutElement.inputIndex, 
            layoutElement.numComponents,
            ToGL(layoutElement.valueType),
            layoutElement.isNormalized,
            layoutElement.stride,
            (GLvoid*)layoutElement.relativeOffset));
    }

    if (geo.desc.isIndexed) {
        OKAMI_EXP_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *geo.indexBuffer));
    }

    OKAMI_EXP_GL(glBindVertexArray(0));

    return geo;
}

Expected<GLGeometry> GLGeometry::Create(Geometry&& geometry) {
    return Create(geometry);
}