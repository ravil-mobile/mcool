#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"


TEST(Simple, LeadingSingleLineComment) {
  ASSERT_THAT(true, true);
}

