cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("renderlib")

# resources
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")

# Укажите путь к Vulkan SDK (обновите путь на ваш)
set(Vulkan_SDK "E:/Vulkan") # Замените на ваш путь к Vulkan SDK

# Добавьте пути к заголовкам Vulkan
include_directories(${Vulkan_SDK}/include)

# Добавьте путь для библиотек Vulkan
link_directories(${Vulkan_SDK}/lib)

# Добавьте источник в исполняемый файл этого проекта.
add_executable(${PROJECT_NAME}
        Scene/Camera.cpp
        Scene/SceneTests/CameraTest.cpp
)

configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/core/config.h @ONLY)

# Собираем шейдеры
set(BUILD_SHADERS_SCRIPT "${CMAKE_SOURCE_DIR}/build_shaders.py")
set(SHADERS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(SHADERS_DEST_DIR "${CMAKE_BINARY_DIR}/shaders")

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Running build_shaders.py script..."
        COMMAND ${CMAKE_COMMAND} -E env PROJECTDIR=${CMAKE_SOURCE_DIR} python ${BUILD_SHADERS_SCRIPT}
        COMMENT "Running build_shaders.py"
)

file(GLOB SPV_SHADERS "${SHADERS_SOURCE_DIR}/*.spv")

foreach(SPV_FILE ${SPV_SHADERS})
  get_filename_component(SPV_FILENAME ${SPV_FILE} NAME)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E echo "Copying ${SPV_FILENAME} to build directory..."
          COMMAND ${CMAKE_COMMAND} -E copy
          ${SPV_FILE} ${SHADERS_DEST_DIR}/${SPV_FILENAME}
          COMMENT "Copying ${SPV_FILENAME} to build directory"
  )
endforeach()

# Компиляция C++
file(GLOB_RECURSE SOURCES "src/*.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include/scene  # Путь к Camera.h
)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

# Подключение библиотек
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

find_package(flecs CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE $<IF:$<TARGET_EXISTS:flecs::flecs>,flecs::flecs,flecs::flecs_static>)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

find_package(vk-bootstrap CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE vk-bootstrap::vk-bootstrap)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)

# Подключение VulkanMemoryAllocator
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan GPUOpen::VulkanMemoryAllocator)

# imgui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/vendor/imgui)
add_library(imgui
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
)
target_include_directories(imgui PUBLIC ${IMGUI_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

find_package(Stb REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR})

find_package(fastgltf CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fastgltf::fastgltf)

# Google Test
set(GTEST_DIR "C:/vcpkg/installed/x64-windows/share/gtest")
find_package(GTest CONFIG REQUIRED PATHS ${GTEST_DIR} NO_DEFAULT_PATH)

include_directories("C:/vcpkg/installed/x64-windows/include")
include_directories("${CMAKE_SOURCE_DIR}/include")  # Путь к вашим заголовкам
include_directories("${CMAKE_SOURCE_DIR}/include/scene")  # Путь к Camera.h

# Добавьте исполняемый файл для тестов
add_executable(CameraTest Scene/SceneTests/CameraTest.cpp)

# Включите директории для тестов
target_include_directories(CameraTest PRIVATE ${GTEST_INCLUDE_DIRS})

# Свяжите с gtest
target_link_libraries(CameraTest PRIVATE GTest::gtest GTest::gtest_main)

enable_testing()
add_test(NAME CameraTests COMMAND CameraTest)
