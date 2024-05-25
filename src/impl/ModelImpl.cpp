// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <cassert>
#include <cstring>
#include <iostream>

#include "impl/ModelImpl.h"

namespace {

}

/*constexpr unsigned short kVID = 0x048d;
constexpr unsigned short kPID = 0x6006;

constexpr unsigned char kGetEffectByte = 0x88_uc;
constexpr unsigned char kGetMonocolorByte = 0x94_uc;

constexpr unsigned kReportLength = 9;

void logAndThrowRuntimeErr(hid_device* const dev)
{
std::wclog << hid_error(dev) << '\n';
throw ChipException("Can't handle the chip (check log)");
}

void checkReportError(int const res, hid_device* const dev)
{
if (res == -1)
{
    logAndThrowRuntimeErr(dev);
}
}

void getReport(hid_device* const dev, unsigned char const controlByte, unsigned char* buf)
{
buf[0] = controlByte;
int res = hid_send_feature_report(dev, buf, kReportLength);
checkReportError(res, dev);
std::memset(buf, 0, kReportLength);
buf[0] = 0;
res = hid_get_feature_report(dev, buf, kReportLength);
checkReportError(res, dev);
}

uint8_t collectBrightness(hid_device* const dev)
{
assert(dev);

unsigned char buf[kReportLength];
getReport(dev, kGetEffectByte, buf);
return buf[5];
}

Color collectRGB(hid_device* const dev)
{
assert(dev);

unsigned char hidBuf[kReportLength];
getReport(dev, kGetMonocolorByte, hidBuf);

Color Color {
    hidBuf[4], // .R
    hidBuf[5], // .G
    hidBuf[6] // .B
};
return Color;
}

hid_device* openDevice()
{
int const res = hid_init();
checkReportError(res, nullptr);

auto* const dev = hid_open(kVID, kPID, nullptr);
if (!dev)
{
    logAndThrowRuntimeErr(nullptr);
}
return dev;
}

}




hid_device* ModelImpl::getChipHandler() const noexcept
{
return ModelImpl::_dev;
}

unsigned char ModelImpl::getBrightness()
{
ModelImpl::_brightness = collectBrightness(_dev);
return ModelImpl::_brightness;
}


Color ModelImpl::getRGB()
{
ModelImpl::_rgb = collectRGB(_dev);
return _rgb;
}*/

ModelImpl::~ModelImpl() {
    _engine.cleanup();
}

ModelImpl::ModelImpl() = default;

void ModelImpl::registerWindow(struct SDL_Window *window) {
    _engine.mainCamera = &_camera;
    _engine.init(window);
}

void ModelImpl::updateVulkan() {
    _engine.update();
}

Camera* ModelImpl::getCamera() {
    return &_camera;
}

/*        : _dev { openDevice() }
        , _brightness { collectBrightness(_dev) }
        , _rgb { collectRGB(_dev) }
{*/