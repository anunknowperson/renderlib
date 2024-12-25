set(BUILD_SHADERS_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/build_shaders.py")

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Running build_shaders.py script..."
    COMMAND ${CMAKE_COMMAND} -E env PROJECTDIR=${CMAKE_SOURCE_DIR} python ${BUILD_SHADERS_SCRIPT}
    COMMENT "Running build_shaders.py")