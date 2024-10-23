include(addGTest)

# Find and include GoogleTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Add your test executable
add_executable(CameraControllerTest CameraControllerTest.cpp)

# Link test executable against gtest & gtest_main
target_link_libraries(CameraControllerTest ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} pthread)