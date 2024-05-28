#pragma once

#include "interfaces/IModel.h"
#include "graphics/vulkan/vk_engine.h"
#include "scene/Camera.h"

class ModelImpl : public IModel
{
public:
	/*! \brief
	 * throws std::runtime_error()
	 */
	ModelImpl();
	~ModelImpl() override;

	ModelImpl(const ModelImpl&) = delete;
	ModelImpl& operator=(const ModelImpl&) = delete;

    void registerWindow(struct SDL_Window* window) override;
    void updateVulkan() override;

    void createMesh(std::string name) override;
    void setMeshTransform(std::string name, glm::mat4x4 transform) override;


	/*[[nodiscard]] hid_device* getChipHandler() const noexcept override;
	unsigned char getBrightness() override;
	Color getRGB() override;*/

    Camera* getCamera() override;

private:
	/*hid_device* _dev;
	uint8_t _brightness;
	Color _rgb;*/

    std::unordered_map<std::string, std::shared_ptr<Mesh>> meshes;

    VulkanEngine _engine;

    Camera _camera;

};