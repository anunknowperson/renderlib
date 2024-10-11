add_subdirectory(core)
add_subdirectory(graphics)
add_subdirectory(scene)

# resources
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
configure_file(config.h.in src/include/core/config.h @ONLY)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/src/include"
)
