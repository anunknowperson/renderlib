include(addGTest)

# Add test target for test_model.cpp
add_gtest(test_model test_model.cpp)

# Add test target for test_camera.cpp
add_gtest(test_camera test_camera.cpp)

# Add test target for test_parent_system.cpp
add_gtest(test_parent_system test_parent_system.cpp)