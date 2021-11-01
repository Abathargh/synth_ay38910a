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

void test_ay38910_play_note(void)
{
	ay38910_play_note(CHANNEL_A, 20);
}

#endif // TEST
