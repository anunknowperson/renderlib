#pragma once

/*! \brief
 *
 * Interface to show user their interaction abilities
 * to see and change backlight settings
 */
class IView
{
public:
	virtual ~IView() = default;

    virtual void run() const = 0;

	/*! \brief
	 * Prints in console current settings
	 * (level of brightness and RGB)
	 */
	//virtual void showCurrentSettings() const = 0;

	/*! \brief
	 * Runs user interface that helps user interact with backlight
	 */
	//virtual void runMenu() const = 0;

	using Ptr = std::unique_ptr<IView>;
};
