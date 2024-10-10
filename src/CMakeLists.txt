add_library(${PROJECT_NAME})

add_subdirectory(core)
add_subdirectory(graphics)
add_subdirectory(scene)

# resources
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
configure_file(config.h.in include/core/config.h @ONLY)

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

target_include_directories(${PROJECT_NAME}
        PUBLIC
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/src/include"
)

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

find_package(VulkanMemoryAllocator CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan GPUOpen::VulkanMemoryAllocator)

find_package(imgui REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui::imgui)

find_package(Stb REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC ${Stb_INCLUDE_DIR})

find_package(fastgltf CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC fastgltf::fastgltf)