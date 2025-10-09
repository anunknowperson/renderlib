#pragma once

#include <cstdint>
#include <expected>
#include <memory>
#include <string_view>

#include "SDL2/SDL.h"
#include "core/MeshController.h"

/*! \brief
 * Interface for interacting with the chip that allows changing keyboard
 * backlight settings.
 *
 * The IController class provides an interface for controlling the keyboard
 * backlight settings. It includes methods for initialization, updating the
 * controller status, processing events, changing the brightness level, and
 * changing the color of the backlight.
 */

class IController {
public:
    /*!
     * \brief Virtual destructor for the interface.
     */
    virtual ~IController() = default;

    /*!
     * \brief Initializes the controller.
     *
     * This method initializes the necessary components for the controller.
     */
    virtual void init() = 0;

    /*!
     * \brief Sends data to change the brightness of the backlight.
     *
     * \param lvl Level of brightness [0; 100].
     *
     * This method sends data to the chip to change the brightness level of the
     * backlight. The lvl parameter specifies the brightness level from 0 (off)
     * to 100 (maximum brightness).
     */
    // virtual void setBrightness(uint16_t lvl) const = 0;

    /*!
     * \brief Sends data to change the color of the backlight.
     *
     * \param color Struct with RGB values (each [0; 255]).
     *
     * This method sends data to the chip to change the color of the backlight.
     * The color parameter is a struct containing RGB values, each ranging from
     * 0 to 255.
     */
    // virtual void setColor(Color color) const = 0;
    [[nodiscard]] virtual MeshController& getMeshController() = 0;
    /*!
     * \brief Shared pointer type for IController.
     */
    using Ptr = std::shared_ptr<IController>;
};

IController::Ptr createInstance();
