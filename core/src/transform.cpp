#include <okami/transform.hpp>

#include <array>
#include <algorithm>

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

glm::vec3 okami::operator*(Transform const& a, glm::vec3 const& b) {
    return a.TransformPoint(b);
}

glm::vec4 okami::operator*(Transform const& a, glm::vec4 const& b) {
    return a.TransformHomogenous(b);
}

Transform okami::Inverse(Transform const& transform) {
    auto conj = glm::conjugate(transform.rotation);
    return Transform(
        glm::rotate(conj, -transform.translation),
        conj,
        1.0f / transform.scale
    );
}

glm::mat4 okami::Transform::ToMatrix4x4() const {
    auto mat = glm::mat4_cast(rotation);
    mat[0] *= scale;
    mat[1] *= scale;
    mat[2] *= scale;
    mat[3] = glm::vec4(translation, 1.0f);
    return mat;
}

Transform Transform::Translate(glm::vec3 t) {
    return Transform(t);
}
Transform Transform::Translate(float x, float y, float z) {
    return Translate(glm::vec3(x, y, z));
}
Transform Transform::Translate(float x, float y) {
    return Translate(glm::vec3(x, y, 0.0f));
}
Transform Transform::Rotate(glm::quat q) {
    return Transform(glm::zero<glm::vec3>(), q);
}
Transform Transform::Rotate2D(float radians) {
    return Rotate(glm::angleAxis(radians, glm::vec3(0.0f, 0.0f, 1.0f)));
}
Transform Transform::Scale(float scale) {
    return Transform(glm::zero<glm::vec3>(), glm::identity<glm::quat>(), scale);
}

glm::vec3 MaxNormColumn(glm::mat3 const& mat) {
    auto magnitudes = std::array{
        glm::length2(mat[0]), glm::length2(mat[1]), glm::length2(mat[2])};
    auto best = std::max_element(magnitudes.begin(), magnitudes.end());
    return mat[best - magnitudes.begin()];
}

glm::quat ToQuaternion(glm::mat3 rotation) {
    auto data = rotation;

    data[0][0] -= 1.0f;
    data[1][1] -= 1.0f;
    data[2][2] -= 1.0f;

    // Gramm-Schmidt
    auto vec1 = MaxNormColumn(data);
    if (glm::l2Norm(vec1) <= std::numeric_limits<float>::epsilon()) {
        return glm::identity<glm::quat>();
    }
    vec1 = glm::normalize(vec1);

    data = data - glm::outerProduct(vec1, vec1) * data;
    auto vec2 = glm::normalize(MaxNormColumn(data));

    auto axis = glm::cross(vec1, vec2);

    float cosTheta = (glm::dot(vec1, rotation * vec1) + glm::dot(vec2, rotation * vec2)) / 2.0f;
    float sinTheta = (glm::dot(vec2, rotation * vec1) - glm::dot(vec1, rotation * vec2)) / 2.0f;

    float theta = glm::atan(sinTheta, cosTheta);

    return glm::angleAxis(theta, axis);
}

Transform Transform::LookAt(glm::vec3 eye, glm::vec3 target, glm::vec3 up) {
    auto forward = glm::normalize(target - eye);
    up = glm::normalize(up);
    auto side = glm::normalize(-glm::cross(forward, up));
    up = -glm::cross(side, forward);
    
    auto rotation = ToQuaternion(glm::mat3{side, up, forward});
    return Transform(eye, rotation);
}