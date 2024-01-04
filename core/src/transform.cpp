#include <okami/transform.hpp>

using namespace okami;

Transform& okami::Transform::operator*=(Transform const& other) {
    translation += scale * (rotation * other.translation);
    rotation = rotation * other.rotation;
    scale *= other.scale;
    return *this;
}

glm::vec3 okami::Transform::TransformPoint(glm::vec3 p) const {
    return scale * (rotation * p) + translation;
}

glm::vec3 okami::Transform::TransformTangent(glm::vec3 t) const {
    return scale * (rotation * t);
}

glm::vec3 okami::Transform::TransformNormal(glm::vec3 n) const {
    return (rotation * n) / scale;
}

glm::vec4 okami::Transform::TransformHomogenous(glm::vec4 v) const {
    return glm::vec4(
        scale * (rotation * glm::vec3(v.x, v.y, v.z)) + v.w * translation,
        v.w);
}

Transform okami::Transform::Inverse() const {
    return ::Inverse(*this);
}

Transform okami::operator*(Transform const& a, Transform const& b) {
    Transform result = a;
    result *= b;
    return result;
}

glm::vec4 okami::operator*(Transform const& a, glm::vec4 const& b) {
    return a.TransformHomogenous(b);
}

Transform okami::Inverse(Transform const& transform) {
    return Transform(
        -transform.translation,
        glm::conjugate(transform.rotation),
        1.0f / transform.scale
    );
}