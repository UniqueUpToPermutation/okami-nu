#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace okami {
    struct Transform {
        glm::vec3 translation;
        float scale;
        glm::quat rotation;

        explicit inline Transform(glm::vec3 translation,
            glm::quat rotation = glm::identity<glm::quat>(),
            float scale = 1.0f) : 
            translation(translation), 
            rotation(rotation), 
            scale(scale) {}
        inline Transform() : Transform(glm::zero<glm::vec3>()) {}

        Transform& operator*=(Transform const& other);
    
        glm::vec3 TransformPoint(glm::vec3 p) const;
        glm::vec3 TransformTangent(glm::vec3 t) const;
        glm::vec3 TransformNormal(glm::vec3 n) const;
        glm::vec4 TransformHomogenous(glm::vec4 v) const;

        Transform Inverse() const;

        static Transform Translate(glm::vec3 t);
        static Transform Translate(float x, float y, float z);
        static Transform Translate(float x, float y);
        static Transform Rotate(glm::quat q);
        static Transform Rotate2D(float radians);
        static Transform Scale(float scale);
        static Transform LookAt(glm::vec3 eye, glm::vec3 target, glm::vec3 up);

        glm::mat4x4 ToMatrix4x4() const;
    };

    Transform operator*(Transform const& a, Transform const& b);
    glm::vec4 operator*(Transform const& a, glm::vec4 const& b);
    glm::vec3 operator*(Transform const& a, glm::vec3 const& b);

    Transform Inverse(Transform const& transform);
}