#pragma once

#include "spdlog/fmt/fmt.h"
#include "spdlog/spdlog.h"

#define LOGGER_FORMAT "[%^%l%$] %v"
#define PROJECT_NAME "renderlib"

// Mainly for IDEs
#ifndef ROOT_PATH_SIZE
#define ROOT_PATH_SIZE 0
#endif

#define __FILENAME__ (static_cast<const char *>(__FILE__) + ROOT_PATH_SIZE)

#define LOGI(...) spdlog::info(__VA_ARGS__);
#define LOGW(...) spdlog::warn(__VA_ARGS__);
#define LOGD(...) spdlog::debug(__VA_ARGS__);

#define LOGIF(...) spdlog::info(fmt::format(__VA_ARGS__));
#define LOGWF(...) spdlog::warn(fmt::format(__VA_ARGS__));
#define LOGDF(...) spdlog::debug(fmt::format(__VA_ARGS__));

#define LOGE(...) spdlog::error("[{}:{}] {}", __FILENAME__, __LINE__, fmt::format(__VA_ARGS__));