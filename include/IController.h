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

    [[nodiscard]] virtual std::weak_ptr<const MeshController> getMeshController() = 0;
    /*!
     * \brief Shared pointer type for IController.
     */
    using Ptr = std::shared_ptr<IController>;
};

IController::Ptr createInstance();
