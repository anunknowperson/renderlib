#pragma once

#include <memory>

struct Event {
    virtual ~Event() = default;
};

/*! \brief
 *
 * Interface to show user their interaction abilities
 * to see and change backlight settings
 */
class IView {
public:
    virtual ~IView() = default;

    virtual void render() = 0;
    using Ptr = std::unique_ptr<IView>;
};
