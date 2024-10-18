cmake_minimum_required (VERSION 3.24)

project ("renderlib")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(options)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

add_library(${PROJECT_NAME})

add_subdirectory(src)
include(linkPackages)

if (BUILD_DEMO)
  add_subdirectory(demo)
endif ()

if (BUILD_SHADERS)
  include(compileShaders)
endif ()

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND ENABLE_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif ()
