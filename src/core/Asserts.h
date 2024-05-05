#pragma once

#include "core/Defines.h"
#include "core/Logging.h"
#include <filesystem>



// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#define RL_INTERNAL_ASSERT_IMPL(check, msg, ...) { if(!(check)) { LOGE(msg, __VA_ARGS__); RL_DEBUGBREAK(); } }
#define RL_INTERNAL_ASSERT_WITH_MSG(check, ...) RL_INTERNAL_ASSERT_IMPL(check, "Assertion failed: {0}", __VA_ARGS__)
#define RL_INTERNAL_ASSERT_NO_MSG(check) RL_INTERNAL_ASSERT_IMPL(check, "Assertion '{0}' failed at {1}:{2}", RL_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define RL_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define RL_INTERNAL_ASSERT_GET_MACRO(...) RL_EXPAND_MACRO( RL_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, RL_INTERNAL_ASSERT_WITH_MSG, RL_INTERNAL_ASSERT_NO_MSG) )

// Currently accepts at least the condition and one additional parameter (the message) being optional
#define RL_ASSERT(...) RL_EXPAND_MACRO( RL_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#define RL_CORE_ASSERT(...) RL_EXPAND_MACRO( RL_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )