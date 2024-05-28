// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <charconv>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <variant>
#include <thread>
#include <chrono>

#include <SDL.h>
#include <SDL_vulkan.h>

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_vulkan.h"

#include "impl/ViewImpl.h"
#include "graphics/vulkan/vk_engine.h"

/*
namespace
{
constexpr char kChangeBrightness = 'b';
constexpr char kChangeMonocolor = 'm';
constexpr char kShowCurrentSettings = 'c';
constexpr char kQuit = 'q';

void printOptions()
{
	std::cout << "OPTIONS\n"
				 "> b     change brightness\n"
				 "> m     change monocolor\n"
				 "> c     show current settings\n"
				 "> q     quit\n";
}

void clearCin()
{
	if (std::cin.eof())
	{
		throw std::runtime_error("unexpected end of program due to EOF (Ctrl + D)");
	}
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void changeBrightness(IController::Ptr const& _controller)
{
	uint16_t lvl; // lvl is not uint8_t, because it reads as character, not number
	std::cout << "value [0, 100]: ";
	std::cin >> lvl;
	if (!std::cin)
	{
		clearCin();
		std::cout << "INVALID ARGUMENT\n";
		return;
	}
	if (lvl <= 100)
	{
		_controller->setBrightness(lvl);
		return;
	}
	std::cout << "LEVEL OF BRIGHTNESS MUST BE FROM 0 TO 100 INCLUSIVELY\n";
}

enum class ColorErrorCode
{
	InvalidArgument,
	OutOfRange,
	NotThreeTokens
};

std::variant<std::array<std::string, 3>, ColorErrorCode> splitColor(std::string const& s)
{
	std::stringstream ss(s);
	std::string token;
	std::array<std::string, 3> colors;
	uint32_t tokensCount = 0;
	while (std::getline(ss, token, '.'))
	{
		if (tokensCount == 3)
		{
			return ColorErrorCode::NotThreeTokens;
		}
		colors[tokensCount++] = token;
	}
	if (tokensCount != 3)
	{
		return ColorErrorCode::NotThreeTokens;
	}
	return colors;
}

std::variant<Color, ColorErrorCode> colorsStringArrToStruct(std::array<std::string, 3> const& tokens)
{
	uint8_t value;
	std::array<uint8_t, 3> colors {};
	uint8_t colorCount = 0;
	for (auto const& i : tokens)
	{
		auto [ptr, ec] = std::from_chars(i.data(), i.data() + i.size(), value);
		if (ec == std::errc())
		{
			colors[colorCount++] = value;
		}
		else if (ec == std::errc::invalid_argument)
		{
			return ColorErrorCode::InvalidArgument;
		}
		else if (ec == std::errc::result_out_of_range)
		{
			return ColorErrorCode::OutOfRange;
		}
	}
	Color Color {
		colors[0],
		colors[1],
		colors[2]
	};
	return Color;
}

void changeColor(IController::Ptr const& c_ptr)
{
	std::string input;
	std::cout << "[R.G.B]: ";
	std::getline(std::cin, input);

	auto const res = splitColor(input);
	if (std::holds_alternative<std::array<std::string, 3>>(res))
	{
		auto const res2 = colorsStringArrToStruct(std::get<std::array<std::string, 3>>(res));
		if (std::holds_alternative<Color>(res2))
		{
			c_ptr->setColor(std::get<Color>(res2));
		}
		else if (std::get<ColorErrorCode>(res2) == ColorErrorCode::InvalidArgument)
		{
			std::cout << "INVALID ARGUMENT\n";
		}
		else if (std::get<ColorErrorCode>(res2) == ColorErrorCode::OutOfRange)
		{
			std::cout << "PRIMARY COLOR VALUE MUST BE FROM 0 TO 255 INCLUSIVELY\n";
		}
	}
	else
	{
		std::cout << "UNABLE TO GET THREE COLORS: EXPECTED THREE TOKENS SEPARATED BY DOT\n";
	}
}

}


}

void ViewImpl::showCurrentSettings() const
{
	auto const lvl = _model->getBrightness();
	Color const RGB = _model->getRGB();
	std::cout << "CURRENT SETTINGS" << '\n'
			  << "> Brightness: " << +lvl << '%' << '\n'
			  << "> RGB: " << +RGB.R << ' ' << +RGB.B << ' ' << +RGB.G << '\n';
}

void ViewImpl::runMenu() const
{
	printOptions();
	std::string optionBuf(1, '0');
	while (optionBuf[0] != kQuit)
	{
		std::cout << "Enter command: ";
		std::cin >> optionBuf;
		if (!std::cin)
		{
			clearCin();
			continue;
		}
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		switch (optionBuf[0])
		{
		case kChangeBrightness:
			changeBrightness(_controller);
			break;
		case kChangeMonocolor:
			changeColor(_controller);
			break;
		case kShowCurrentSettings:
			showCurrentSettings();
			break;
		case kQuit:
			break;
		default:
			std::cout << "WRONG OPTION\n";
			break;
		}
	}
}*/

ViewImpl::ViewImpl(IController::Ptr controller, IModel::Ptr model) : _controller(std::move(controller)),
                                                                     _model(std::move(model)) {

    SDL_Init(SDL_INIT_VIDEO);

    auto window_flags = (SDL_WindowFlags) (SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);



    window = SDL_CreateWindow(
            "engine",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            1700,
            900,
            window_flags);



}


void ViewImpl::run() const {

    _model->registerWindow(window);


    _controller->init();

    SDL_Event e;
    bool bQuit = false;

    bool stop_rendering = false;

    // main loop
    while (!bQuit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // close the window when user alt-f4s or clicks the X button
            if (e.type == SDL_QUIT)
                bQuit = true;

            if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                    stop_rendering = true;
                }
                if (e.window.event == SDL_WINDOWEVENT_RESTORED) {
                    stop_rendering = false;
                }
            }

            _controller->processEvent(e);
            ImGui_ImplSDL2_ProcessEvent(&e);

        }

        // do not draw if we are minimized
        if (stop_rendering) {
            // throttle the speed to avoid the endless spinning
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // imgui new frame
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if (ImGui::Begin("background")) {

            VulkanEngine& engine = VulkanEngine::Get();
            ImGui::SliderFloat("Render Scale",&engine.renderScale, 0.3f, 1.f);
            //other code
        }
        ImGui::End();

        //make imgui calculate internal draw structures
        ImGui::Render();


        _controller->update();
        //_model->updateVulkan();

        //engine.update();

    }

}

ViewImpl::~ViewImpl() {
    SDL_DestroyWindow(window);
}
