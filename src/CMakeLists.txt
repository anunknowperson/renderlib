add_library(SRC STATIC)

add_subdirectory(core)
add_subdirectory(graphics)
add_subdirectory(scene)

add_subdirectory(imgui)

target_link_libraries(SRC PUBLIC IMGUI_SRC)

target_include_directories(SRC
        PUBLIC
        "${CMAKE_SOURCE_DIR}/include"
)
