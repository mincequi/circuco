#include <unity.h>

#include <Util.hpp>

void to_duration(void) {
    TEST_ASSERT_EQUAL_STRING("null", toDuration(0, "null").c_str());
    TEST_ASSERT_EQUAL_STRING("1 s", toDuration(1, "null").c_str());
    TEST_ASSERT_EQUAL_STRING("1 min", toDuration(60, "null").c_str());
    TEST_ASSERT_EQUAL_STRING("1 min 2 s", toDuration(62, "null").c_str());
    TEST_ASSERT_EQUAL_STRING("1 h", toDuration(3600, "null").c_str());
    TEST_ASSERT_EQUAL_STRING("1 h 1 min 2 s", toDuration(3662, "null").c_str());
}

void to_uptime() {
    TEST_ASSERT_EQUAL_STRING("0 h", toUptime(1000 * 60 * 60 * 0).c_str());
    TEST_ASSERT_EQUAL_STRING("1 h", toUptime(1000 * 60 * 60 * 1).c_str());
    TEST_ASSERT_EQUAL_STRING("1 d 0 h", toUptime(1000 * 60 * 60 * 24 + 1).c_str());
    TEST_ASSERT_EQUAL_STRING("1 d 2 h", toUptime(1000 * 60 * 60 * 26).c_str());
}

auto runUtilTest() {
    UNITY_BEGIN();
    RUN_TEST(to_duration);
    RUN_TEST(to_uptime);
    UNITY_END();
}