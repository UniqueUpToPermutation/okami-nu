#pragma once

#include <glm/glm.hpp>
#include <variant>

#include <glm/gtc/constants.hpp>

#include <okami/transform.hpp>

#include <string_view>

namespace okami {
    namespace prototypes {
        constexpr std::string_view Camera = "protoCamera";
    }

    enum class CameraType {
        Perspective,
        Orthographic
    };

    struct CameraVariantOrtho {
    };

    struct CameraVariantPerspective {
        float fieldOfView = glm::pi<float>();
    };

    using CameraVariant = std::variant<CameraVariantOrtho, CameraVariantPerspective>;

    glm::mat4x4 OrthoProjection(glm::vec2 viewport, float near, float far);
    glm::mat4x4 PerspectiveProjection(float fieldOfView, glm::vec2 viewport, float near, float far);
    glm::mat4x4 Projection(CameraVariant const& props, glm::vec2 viewport, float near, float far);

    struct Camera {
        CameraVariant variant;
        float near = 0.1f;
        float far = 100.0f;

        glm::mat4x4 GetProjMatrix(glm::vec2 viewport) const;
    };

    struct CameraRenderData {
        Camera camera;
        glm::vec2 viewport;
        Transform viewTransform;

        glm::mat4x4 GetProjMatrix() const;
        inline glm::mat4x4 GetViewMatrix() const {
            return viewTransform.ToMatrix4x4();
        }
        inline glm::mat4x4 GetViewProjMatrix() const {
            return GetViewMatrix() * GetProjMatrix();
        }
    };
}