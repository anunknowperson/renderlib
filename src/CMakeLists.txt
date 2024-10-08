add_library(${PROJECT_NAME})

add_subdirectory(core)
add_subdirectory(graphics)
add_subdirectory(scene)

target_include_directories(src
        PUBLIC
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/src/include"
)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)

find_package(flecs CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC $<IF:$<TARGET_EXISTS:flecs::flecs>,flecs::flecs,flecs::flecs_static>)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

find_package(vk-bootstrap CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC vk-bootstrap::vk-bootstrap)

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan)

find_package(VulkanMemoryAllocator CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Vulkan::Vulkan GPUOpen::VulkanMemoryAllocator)

find_package(imgui REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui::imgui)

find_package(Stb REQUIRED)
target_include_directories(src PUBLIC ${Stb_INCLUDE_DIR})

find_package(fastgltf CONFIG REQUIRED)
target_link_libraries(src PUBLIC fastgltf::fastgltf)