cmake_minimum_required (VERSION 3.13)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("renderlib")

# resources
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME})
add_subdirectory(src)
target_link_libraries(${PROJECT_NAME} PUBLIC src)

configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/incude/core/config.h @ONLY)

# Собираем шейдеры
set(BUILD_SHADERS_SCRIPT "${CMAKE_SOURCE_DIR}/build_shaders.py")

set(SHADERS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(SHADERS_DEST_DIR "${CMAKE_BINARY_DIR}/shaders")

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Running build_shaders.py script..."
        COMMAND ${CMAKE_COMMAND} -E env PROJECTDIR=${CMAKE_SOURCE_DIR} python ${BUILD_SHADERS_SCRIPT}
        COMMENT "Running build_shaders.py")

file(GLOB SPV_SHADERS "${SHADERS_SOURCE_DIR}/*.spv")

foreach(SPV_FILE ${SPV_SHADERS})
  get_filename_component(SPV_FILENAME ${SPV_FILE} NAME)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E echo "Copying ${SPV_FILENAME} to build directory..."
          COMMAND ${CMAKE_COMMAND} -E copy
          ${SPV_FILE} ${SHADERS_DEST_DIR}/${SPV_FILENAME}
          COMMENT "Copying ${SPV_FILENAME} to build directory")
endforeach()

add_executable(${PROJECT_NAME}.x main.cpp)
target_link_libraries(${PROJECT_NAME}.x ${PROJECT_NAME})
