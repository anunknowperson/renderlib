#include <gtest/gtest.h>

#include "core/model.h"  // Header for createModel()

// Test for createModel() factory function
TEST(ModelTest, CreateModel) {
    auto model = createModel();
    ASSERT_NE(model, nullptr) << "Model should not be nullptr";

    EXPECT_NO_THROW({ model->registerWindow(nullptr); });
}

// Entry point for running tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
