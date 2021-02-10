#include "gtest/gtest.h"
#include "lib/service.h"

TEST(HelloTest, GetGreet) {
  EXPECT_EQ(greet("Bazel"), "Hello Bazel");
}

TEST(HelloTest, GetGreetFAIL) {
  EXPECT_NE(greet("Jean"), "Hello Bazel");
}