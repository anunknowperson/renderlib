#pragma once

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include "glm/gtx/quaternion.hpp"
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <vector>

struct LocalTransformComponent {
    glm::vec3 Position;
    glm::quat Rotation;
    glm::float64 Scale;


    static LocalTransformComponent FromMatrix(const glm::mat4 &matrix);
    static LocalTransformComponent FromPosition(const glm::vec3 &position);
    static LocalTransformComponent FromRotation(const glm::quat &rotation);
    static LocalTransformComponent FromScale(const glm::float64 scale);

    static glm::mat4 ToMatrix(const LocalTransformComponent &transform);

    void Rotate(const glm::quat &rotation);
    void RotateX(const glm::float64 rotation);
    void RotateY(const glm::float64 rotation);
    void RotateZ(const glm::float64 rotation);
    void Translate(const glm::vec3 &translation);
    void TranslateX(const glm::float64 translation);
    void TranslateY(const glm::float64 translation);
    void TranslateZ(const glm::float64 translation);
    void SetScale(const glm::float64 scale);
    void TransformTransform(const LocalTransformComponent &transform);
    void Inverse();
    void TransformPoint(const glm::vec3 &point);
    void TransformDirection(const glm::vec3 &direction);
};
