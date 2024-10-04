add_library(IMGUI_SRC
        imgui_impl_sdl2.cpp
        imgui_impl_vulkan.cpp
        imgui_impl_sdl2.h
        imgui_impl_vulkan.h
)
target_include_directories(IMGUI_SRC PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/../../vendor/imgui
)