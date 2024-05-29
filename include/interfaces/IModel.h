#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <string_view>

#include <SDL.h>
#include <SDL_vulkan.h>
#include "scene/Camera.h"


/*! \brief
 * Interface for initialising the work with chip
 * and getting settings from the chip
 */
class IModel
{
public:
	virtual ~IModel() = default;

    virtual void registerWindow(struct SDL_Window* window) = 0;

    virtual void updateVulkan() = 0;

    virtual void createMesh(std::string name) = 0;
    virtual void setMeshTransform(std::string name, glm::mat4x4 transform) = 0;

    [[nodiscard]] virtual Camera* getCamera() = 0;

	/*! \brief
	 * Gets the chip handler from HIDAPI required to change the settings by the Controller
	 * @return
	 * Returns the device handler in case, can't be nullptr
	 */
	//[[nodiscard]] virtual hid_device* getChipHandler() const = 0;

	/*! \brief
	 * Requests from chip the level of brightness
	 * @return
	 * Returns the level of brightness [0; 100]
	 */
	//virtual uint8_t getBrightness() = 0;

	/*! \brief
	 * Requests RGB values from the chip
	 * @return
	 * Returns the struct with .R, .G, .B fields
	 */
	//virtual Color getRGB() = 0;

	using Ptr = std::shared_ptr<IModel>;
};
