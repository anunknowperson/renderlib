# CMakeList.txt: проект CMake для renderlib; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("renderlib")

# Добавьте источник в исполняемый файл этого проекта.
add_executable (${PROJECT_NAME})

# Собираем шейдеры

set(BUILD_SHADERS_SCRIPT "${CMAKE_SOURCE_DIR}/build_shaders.py")

set(SHADERS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(SHADERS_DEST_DIR "${CMAKE_BINARY_DIR}/shaders")

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Running build_shaders.py script..."
        COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${CMAKE_SOURCE_DIR} python ${BUILD_SHADERS_SCRIPT}
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

# Компиляция C++

file(GLOB_RECURSE SOURCES "src/*.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()


find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)

find_package(flecs REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE flecs::flecs)
