#include <gtest/gtest.h>

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();

    return result;
}

TEST(Core, SystemTest) {
    EXPECT_TRUE(true);
    EXPECT_FALSE(false);
    EXPECT_TRUE(1 == 1);
    EXPECT_FALSE(1 == 0);
}
