#include "scene/LocalTransformComponent.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/orthonormalize.hpp"
#include "core/logging.h"

LocalTransformComponent LocalTransformComponent::FromMatrix(const glm::mat4 &matrix)
{
    LocalTransformComponent transform;
    glm::vec3 position = matrix[3];

    glm::float64 epsilon = 0.0001;
    glm::float64 scale_x = glm::length(matrix[0]);
    glm::float64 scale_y = glm::length(matrix[1]);
    glm::float64 scale_z = glm::length(matrix[2]);

    if (abs(scale_x - scale_y) > epsilon || abs(scale_x - scale_z) > epsilon || abs(scale_y - scale_z) > epsilon) {
        LOGW("Trying to convert a float4x4 to a LocalTransform, but the scale is not uniform");
    }
    glm::float64 scale = sqrt(scale_x);

    glm::mat3 pos_matrix = glm::mat3(matrix);

    glm::float64 dot_product1 = glm::dot(pos_matrix[0], pos_matrix[1]);
    glm::float64 dot_product2 = glm::dot(pos_matrix[0], pos_matrix[2]);
    glm::float64 dot_product3 = glm::dot(pos_matrix[1], pos_matrix[2]);
    if (abs(dot_product1) > epsilon || abs(dot_product2) > epsilon || abs(dot_product3) > epsilon) {
        LOGW("Trying to convert a float4x4 to a LocalTransform, but the rotation is not orthogonal");
    }

    pos_matrix = glm::orthonormalize(pos_matrix);
    glm::quat rotation = glm::quat_cast(pos_matrix);

    transform.Position = position;
    transform.Rotation = rotation;
    transform.Scale = scale;
    return transform;
}

LocalTransformComponent LocalTransformComponent::FromPosition(const glm::vec3 &position)
{
    LocalTransformComponent transform;
    transform.Position = position;
    transform.Rotation = glm::quat(1, 0, 0, 0);
    transform.Scale = 1;
    return transform;
}

LocalTransformComponent LocalTransformComponent::FromRotation(const glm::quat &rotation)
{
    LocalTransformComponent transform;
    transform.Position = glm::vec3(0, 0, 0);
    transform.Rotation = rotation;
    transform.Scale = 1;
    return transform;
}

LocalTransformComponent LocalTransformComponent::FromScale(const glm::float64 scale)
{
    LocalTransformComponent transform;
    transform.Position = glm::vec3(0, 0, 0);
    transform.Rotation = glm::quat(1, 0, 0, 0);
    transform.Scale = scale;
    return transform;
}

glm::mat4 LocalTransformComponent::ToMatrix(const LocalTransformComponent &transform)
{
    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, transform.Position);
    matrix = matrix * glm::mat4_cast(transform.Rotation);
    matrix = matrix * glm::scale(glm::mat4(1.0f), glm::vec3(transform.Scale));
    return matrix;
}

void LocalTransformComponent::Rotate(const glm::quat &rotation)
{
    this->Rotation = this->Rotation * rotation;
}

void LocalTransformComponent::RotateX(const glm::float64 rotation)
{
    this->Rotation = this->Rotation * glm::quat(glm::vec3(rotation, 0, 0));
}

void LocalTransformComponent::RotateY(const glm::float64 rotation)
{
    this->Rotation = this->Rotation * glm::quat(glm::vec3(0, rotation, 0));
}

void LocalTransformComponent::RotateZ(const glm::float64 rotation)
{
    this->Rotation = this->Rotation * glm::quat(glm::vec3(0, 0, rotation));
}

void LocalTransformComponent::Translate(const glm::vec3 &translation)
{
    this->Position = this->Position + translation;
}

void LocalTransformComponent::TranslateX(const glm::float64 translation)
{
    this->Position = this->Position + glm::vec3(translation, 0, 0);
}

void LocalTransformComponent::TranslateY(const glm::float64 translation)
{
    this->Position = this->Position + glm::vec3(0, translation, 0);
}

void LocalTransformComponent::TranslateZ(const glm::float64 translation)
{
    this->Position = this->Position + glm::vec3(0, 0, translation);
}

void LocalTransformComponent::SetScale(const glm::float64 scale)
{
    this->Scale = scale;
}

void LocalTransformComponent::TransformTransform(const LocalTransformComponent &transform)
{
    this->Position = transform.Position;
    this->Rotation = transform.Rotation;
    this->Scale = transform.Scale;
}

void LocalTransformComponent::Inverse()
{
    glm::quat inverse_rotation = glm::inverse(this->Rotation);
    glm::float64 inverse_scale = 1.0f / this->Scale;
    this->Position = -glm::rotate(inverse_rotation, this->Position) * static_cast<float>(this->Scale);
    this->Rotation = inverse_rotation;
    this->Scale = inverse_scale;
}

void LocalTransformComponent::TransformPoint(const glm::vec3 &point)
{
    this->Position = this->Position + glm::rotate(this->Rotation, point) * static_cast<float>(this->Scale);
}

void LocalTransformComponent::TransformDirection(const glm::vec3 &direction)
{
    this->Position = this->Position + glm::rotate(this->Rotation, direction);
}