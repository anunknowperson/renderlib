// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <cassert>
#include <iostream>
#include <utility>

#include "impl/ControllerImpl.h"

namespace {

}
/* special bytes used for create payload
 * use unsigned char because of C library, that uses this type
 */

/*
constexpr unsigned char kActionByte = 0x02_uc;
constexpr unsigned char kBrightnessByte = 0x09_uc;
constexpr unsigned char kColorByte = 0x14_uc;

constexpr unsigned kReportLength = 9;

void sendFeatureReport(hid_device* const dev, unsigned char const (&payload)[kReportLength])
{
	int const res = hid_send_feature_report(dev, payload, kReportLength);
	if (res == -1)
	{
		std::wclog << hid_error(dev) << '\n';
		throw ChipException("Can't handle the chip (check log)");
	}
}
}



void ControllerImpl::setBrightness(uint16_t const lvl) const
{
	assert(0 <= lvl and lvl <= 100);

	unsigned char payload[kReportLength] {}; // use unsigned char because of C library, that uses this type
	payload[0] = kBrightnessByte;
	payload[1] = kActionByte;
	payload[2] = static_cast<unsigned char>(lvl);
	auto* const dev = _model->getChipHandler();
	sendFeatureReport(dev, payload);
}

void ControllerImpl::setColor(Color const Color) const
{
	assert(0 <= Color.R and Color.R <= 255);
	assert(0 <= Color.B and Color.B <= 255);
	assert(0 <= Color.G and Color.G <= 255);
	unsigned char payload[kReportLength] {}; // use unsigned char because of C library, that uses this type
	payload[0] = kColorByte;
	payload[1] = 0x01_uc;
	payload[2] = 0x01_uc;
	payload[3] = static_cast<unsigned char>(Color.R);
	payload[4] = static_cast<unsigned char>(Color.G);
	payload[5] = static_cast<unsigned char>(Color.B);

	auto* const dev = _model->getChipHandler();
	sendFeatureReport(dev, payload);
}
*/

ControllerImpl::ControllerImpl(IModel::Ptr model)
        : _model(std::move(model))
{

}

void ControllerImpl::update() const {
    _model->updateVulkan();

    _model->getCamera()->update();
}

void ControllerImpl::processEvent(SDL_Event &e) const {
    _model->getCamera()->processSDLEvent(e);
}
