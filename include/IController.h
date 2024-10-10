#pragma once

#include <cstdint>
#include <expected>
#include <memory>
#include <string_view>
#include "SDL2/SDL.h"

/*! \brief
 * Interface for interacting with the chip that allows changing keyboard backlight settings.
 *
 * The IController class provides an interface for controlling the keyboard backlight settings.
 * It includes methods for initialization, updating the controller status, processing events,
 * changing the brightness level, and changing the color of the backlight.
 */

class IController
{
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
    virtual void init() const = 0;

    /*!
     * \brief Updates the controller status.
     *
     * This method updates the internal state or status of the controller.
     */
    virtual void update() const = 0;

    /*!
     * \brief Processes incoming events.
     *
     * \param e Event to be processed.
     *
     * This method processes the incoming SDL event and performs necessary actions.
     */
    virtual void processEvent(SDL_Event &e) const = 0;

    /*!
     * \brief Sends data to change the brightness of the backlight.
     *
     * \param lvl Level of brightness [0; 100].
     *
     * This method sends data to the chip to change the brightness level of the backlight.
     * The lvl parameter specifies the brightness level from 0 (off) to 100 (maximum brightness).
     */
    //virtual void setBrightness(uint16_t lvl) const = 0;

    /*!
     * \brief Sends data to change the color of the backlight.
     *
     * \param color Struct with RGB values (each [0; 255]).
     *
     * This method sends data to the chip to change the color of the backlight.
     * The color parameter is a struct containing RGB values, each ranging from 0 to 255.
     */
    //virtual void setColor(Color color) const = 0;

    /*!
     * \brief Shared pointer type for IController.
     */
    using Ptr = std::shared_ptr<const IController>;
};

IController::Ptr createInstance();
