#include "unity.h"
#include "app_serial.h"

/* This function is called before every test is run */
void setUp(void)
{
    
}

/* This function is called after every test is run */
void tearDown(void)
{

}

// Testing Validate_Time() function
/*-----------------------------------------------------------------------------------------------*/
/* Test case: All values (hour, minutes, and seconds) are valid */
void test_Validate_Time_AllValuesValid(void)
{
    uint8_t result = Validate_Time(12, 30, 30);
    /* Assert that if hour = 12, minutes = 30, and seconds = 30, then the result should be 1 (valid) */
    TEST_ASSERT_EQUAL_UINT8(1, result);
}

/* Test case: Hour value is not valid */
void test_Validate_Time_HourNotValid(void)
{
    uint8_t result = Validate_Time(24, 30, 30);
    /* Assert that if hour = 24 (out of range), the result should be 0 (invalid) */
    TEST_ASSERT_EQUAL_UINT8(0, result);
}

/* Test case: Minutes value is not valid */
void test_Validate_Time_MinutesNotValid(void)
{
    uint8_t result = Validate_Time(12, 60, 30);
    /* Assert that if minutes = 60 (out of range), the result should be 0 (invalid) */
    TEST_ASSERT_EQUAL_UINT8(0, result);
}

/* Test case: Seconds value is not valid */
void test_Validate_Time_SecondsNotValid(void)
{
    uint8_t result = Validate_Time(12, 30, 60);
    /* Assert that if seconds = 60 (out of range), the result should be 0 (invalid) */
    TEST_ASSERT_EQUAL_UINT8(0, result);
}