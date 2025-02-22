include(addGTest)

# add targets by calling add_gtest
add_gtest(dummy_test dummy.cpp)

add_gtest(CameraTest CameraTest.cpp)
add_gtest(CameraControllerTest CameraControllerTest.cpp)