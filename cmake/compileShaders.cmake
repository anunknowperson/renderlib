
set(BUILD_SHADERS_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/build_shaders.py")
set(SHADERS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/shaders")
set(SHADERS_DEST_DIR "${CMAKE_BINARY_DIR}/demo")  # Target directory in build

# Create demo directory if it doesn't exist
file(MAKE_DIRECTORY ${SHADERS_DEST_DIR})

add_custom_command(
    TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Compiling shaders..."
    COMMAND python ${BUILD_SHADERS_SCRIPT} 
        "--source=${SHADERS_SOURCE_DIR}" 
        "--output=${SHADERS_DEST_DIR}"
    COMMENT "Shaders → ${SHADERS_DEST_DIR}"
)