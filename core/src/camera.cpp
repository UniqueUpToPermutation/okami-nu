#include <okami/camera.hpp>
#include <okami/okami.hpp>

#include <glm/gtc/matrix_transform.hpp>

using namespace okami;

glm::mat4x4 okami::OrthoProjection(
    glm::vec2 viewport, float near, float far) {
    return glm::orthoLH_NO(
        -viewport.x / 2.0f, 
        viewport.x / 2.0f, 
        -viewport.y / 2.0f, 
        viewport.y / 2.0f,
        near,
        far);
}
glm::mat4x4 okami::PerspectiveProjection(
    float fieldOfView, glm::vec2 viewport, float near, float far) {
    return glm::perspectiveFovLH_NO(fieldOfView, viewport.x, viewport.y, near, far);
}

glm::mat4x4 okami::Projection(
    CameraVariant const& props, glm::vec2 viewport, float near, float far) {
    return MatchReturn(props,
        [&](CameraVariantOrtho const& props) {
            return OrthoProjection(viewport, near, far);
        },
        [&](CameraVariantPerspective const& props) {
            return PerspectiveProjection(props.fieldOfView, viewport, near, far);
        });
}

glm::mat4x4 okami::Camera::GetProjMatrix(glm::vec2 viewport) const {
    if (extents) {
        return Projection(variant, *extents, near, far);
    } else {
        return Projection(variant, viewport, near, far);
    }
}

glm::mat4x4 okami::RenderView::GetProjMatrix() const {
    return camera.GetProjMatrix(viewport);
}