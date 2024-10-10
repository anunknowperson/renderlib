cmake_minimum_required (VERSION 3.13)

project ("renderlib")

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_SOURCE_DIR}/cmake/options.cmake)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

add_library(${PROJECT_NAME})

add_subdirectory(src)

if (BUILD_SHADERS)
  include(${CMAKE_SOURCE_DIR}/cmake/compileShaders.cmake)
endif ()

if (BUILD_DEMO)
  add_subdirectory(demo)
endif ()
