#include "unity.h"
#include "rtc.h"
#include <string.h>
#include <stdbool.h>

// --- Mocks for hardware access ---
static uint8_t hw_regs[7]; // Simulate RTC registers

bool RTC_HW_Read(uint8_t reg, uint8_t *data, int len) {
    if (reg + len > sizeof(hw_regs)) return false;
    memcpy(data, &hw_regs[reg], len);
    return true;
}

bool RTC_HW_Write(uint8_t reg, const uint8_t *data, int len) {
    if (reg + len > sizeof(hw_regs)) return false;
    memcpy(&hw_regs[reg], data, len);
    return true;
}

// --- Test setup/teardown ---
void setUp(void) {
    memset(hw_regs, 0, sizeof(hw_regs));
}

void tearDown(void) {
    // Nothing needed
}

// --- Helper to initialize fake time ---
static void init_fake_time(void) {
    hw_regs[0] = 0x30; // seconds 30
    hw_regs[1] = 0x45; // minutes 45
    hw_regs[2] = 0x12; // hours 12
    hw_regs[3] = 0x02; // day 2
    hw_regs[4] = 0x15; // date 15
    hw_regs[5] = 0x09; // month 9
    hw_regs[6] = 0x23; // year 2023
}

// --- Test RTC_Init ---
void test_RTC_Init_success(void) {
    hw_regs[0] = 0x00; // seconds register present
    TEST_ASSERT_TRUE(RTC_Init());
}

void test_RTC_Init_fail(void) {
    // Simulate read failure
    bool RTC_HW_Read(uint8_t reg, uint8_t *data, int len) { return false; }
    TEST_ASSERT_FALSE(RTC_Init());
}

// --- Test RTC_ReadTime ---
void test_RTC_ReadTime(void) {
    init_fake_time();
    RTC_Time_t t;
    TEST_ASSERT_TRUE(RTC_ReadTime(&t));
    TEST_ASSERT_EQUAL_UINT8(30, t.seconds);
    TEST_ASSERT_EQUAL_UINT8(45, t.minutes);
    TEST_ASSERT_EQUAL_UINT8(12, t.hours);
    TEST_ASSERT_EQUAL_UINT8(2, t.day);
    TEST_ASSERT_EQUAL_UINT8(15, t.date);
    TEST_ASSERT_EQUAL_UINT8(9, t.month);
    TEST_ASSERT_EQUAL_UINT16(2023, t.year);
}

// --- Test RTC_WriteTime ---
void test_RTC_WriteTime(void) {
    RTC_Time_t t = { .seconds=10, .minutes=20, .hours=8,
                      .day=1, .date=5, .month=11, .year=2025 };
    TEST_ASSERT_TRUE(RTC_WriteTime(&t));

    // Check BCD conversion
    TEST_ASSERT_EQUAL_UINT8(0x10, hw_regs[0]);
    TEST_ASSERT_EQUAL_UINT8(0x20, hw_regs[1]);
    TEST_ASSERT_EQUAL_UINT8(0x08, hw_regs[2]);
    TEST_ASSERT_EQUAL_UINT8(0x01, hw_regs[3]);
    TEST_ASSERT_EQUAL_UINT8(0x05, hw_regs[4]);
    TEST_ASSERT_EQUAL_UINT8(0x11, hw_regs[5]);
    TEST_ASSERT_EQUAL_UINT8(0x25, hw_regs[6]);
}

// --- Test RTC_BurstRead and BurstWrite ---
void test_RTC_BurstReadWrite(void) {
    uint8_t buf[7] = {1,2,3,4,5,6,7};
    TEST_ASSERT_TRUE(RTC_BurstWrite(buf, 7));

    uint8_t read_buf[7] = {0};
    int n = RTC_BurstRead(read_buf, 7);
    TEST_ASSERT_EQUAL_INT(7, n);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(buf, read_buf, 7);
}

// --- Test Start/Stop/IsDone ---
void test_RTC_StartStopIsDone(void) {
    TEST_ASSERT_TRUE(RTC_Start());
    TEST_ASSERT_TRUE(RTC_Stop());
    TEST_ASSERT_FALSE(RTC_IsDone()); // dummy implementation
}

// --- Unity Main ---
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_RTC_Init_success);
    RUN_TEST(test_RTC_ReadTime);
    RUN_TEST(test_RTC_WriteTime);
    RUN_TEST(test_RTC_BurstReadWrite);
    RUN_TEST(test_RTC_StartStopIsDone);

    return UNITY_END();
}
