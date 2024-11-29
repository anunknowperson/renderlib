#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <string_view>

#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"
#include "scene/Camera.h"
#include "scene/Mesh.h"

/*!
 * \brief Interface for managing models and their integration with Vulkan.
 *
 * The IModel class provides an interface for handling models, registering a
 window for Vulkan rendering,
 * updating Vulkan-related states, creating meshes, setting mesh
 transformations, and accessing the camera.

 */
class IModel {
public:
    /*!
     * \brief Virtual destructor for the interface.
     *
     * Ensures proper cleanup of derived classes.
     */
    virtual ~IModel() = default;

    /*!
     * \brief Registers a window for Vulkan rendering.
     *
     * \param window Pointer to an SDL_Window.
     *
     * This method registers the given window for rendering using Vulkan.
     * It is essential to call this method before performing any Vulkan
     * rendering operations.
     */
    virtual void registerWindow(struct SDL_Window* window) = 0;

    /*!
     * \brief Creates a new mesh with the given name.
     *
     * \param engine
     * \param file_path
     *
     * This method creates a new mesh identified by the provided name.
     */
    virtual void createMesh(VulkanEngine& engine, std::string_view file_path) = 0;

    virtual void delete_mesh(Mesh::rid_t rid) = 0;

    /*!
     * \brief Sets the transformation matrix for a mesh.
     *
     * \param rid
     * \param transform Transformation matrix to be applied to the mesh.
     *
     * This method sets the transformation matrix for the mesh identified by the
     * provided name.
     */
    virtual void setMeshTransform(Mesh::rid_t rid, glm::mat4x4 transform) = 0;

    virtual glm::mat4 get_mesh_transform(Mesh::rid_t) = 0;

    struct MeshPair {
     std::shared_ptr<const Mesh::GLTF::LoadedGLTF> ptr;
     glm::mat4 transform;
    };
    using MeshMap = std::unordered_map<Mesh::rid_t, MeshPair>;
    virtual const MeshMap& get_meshes() = 0;

    /*!
     * \brief Retrieves the camera instance.
     *
     * \return Pointer to the Camera instance.
     *
     * This method returns a pointer to the camera instance managed by the
     * model. Can be used to access and modify camera properties.
     */
    [[nodiscard]] virtual Camera* getCamera() = 0;

     virtual VulkanEngine& get_engine() = 0;

    using Ptr = std::shared_ptr<IModel>;
};
