#ifdef TEST

#include "unity.h"

#include "Ay38910a.h"

#include "mock_io.h"

void setUp(void)
{
    ay38910_init();
}

void tearDown(void)
{
}

void test_Ay38910a_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement Ay38910a");
}

#endif // TEST
