#pragma once

#define BIT(x) (1 << x)

#define RL_EXPAND_MACRO(x) x
#define RL_STRINGIFY_MACRO(x) #x

#define RL_DEBUGBREAK() __debugbreak()

#define RL_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }