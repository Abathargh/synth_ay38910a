#ifdef TEST

#include "unity.h"
#include "Ay38910a.h"

#include <time.h>
#include <stdlib.h>
#include <stdint.h>

uint8_t data_dir_mock_a = 0;
uint8_t data_dir_mock_b = 0;

uint8_t port_mock_a = 0;
uint8_t port_mock_b = 0;

const uint8_t pin_mock_bc1 = 1;
const uint8_t pin_mock_bdir = 2;

Pin bc1_pin = {&data_dir_mock_a, &port_mock_a, pin_mock_bc1};
Pin bdir_pin = {&data_dir_mock_a, &port_mock_a, pin_mock_bdir};
Port bus_port = {&data_dir_mock_b, &port_mock_b};

void setUp(void)
{
	data_dir_mock_a = 0;
	data_dir_mock_b = 0;

	port_mock_a = 0;
	port_mock_b = 0;
}

void tearDown(void)
{
}

void test_ay38910_init(void)
{
	ay38910_init(bc1_pin, bdir_pin, bus_port);
	TEST_ASSERT_EQUAL(data_dir_mock_a, 1 << pin_mock_bc1 | 1 << pin_mock_bdir);
	TEST_ASSERT_EQUAL(data_dir_mock_b, 0xFF);
}

void test_ay38910_play_note(void)
{
	uint8_t note = 0;
	Channel chans[] = {CHANNEL_A, CHANNEL_B, CHANNEL_C};
	
	ay38910_init(bc1_pin, bdir_pin, bus_port);

	for(int chan_idx = 0; chan_idx < sizeof chans; chan_idx++)
	{
		ay38910_play_note(chans[chan_idx], note);
		TEST_ASSERT_EQUAL(port_mock_a, 0x00);
	}
}

void test_ay38910_play_nose(void)
{
	ay38910_init(bc1_pin, bdir_pin, bus_port);
	for(int divider = 0; divider < 256; divider++)
	{
		ay38910_play_noise((uint8_t)divider);
		TEST_ASSERT_EQUAL(port_mock_a, 0x00);
		TEST_ASSERT_EQUAL(port_mock_b, 0x1F & divider);	
	}
}

void test_ay38910_channel_mode(void)
{
	uint8_t channel_modes[] = {
		CHA_TONE_ENABLE, CHB_TONE_ENABLE, CHC_TONE_ENABLE,
		CHA_NOISE_ENABLE, CHB_NOISE_ENABLE, CHC_NOISE_ENABLE,
		CHA_TONE_DISABLE, CHB_TONE_DISABLE, CHC_TONE_DISABLE,
		CHA_NOISE_DISABLE, CHB_NOISE_DISABLE, CHC_NOISE_DISABLE,
	};
	
	ay38910_init(bc1_pin, bdir_pin, bus_port);
	
	for(int idx = 0; idx < sizeof channel_modes; idx++)
	{
		ay38910_channel_mode(channel_modes[idx]);
		TEST_ASSERT_EQUAL(port_mock_a, 0x00);
		TEST_ASSERT_EQUAL(port_mock_b, 0xC0 | channel_modes[idx]);
	}
}

void test_ay38910_set_amplitude(void)
{
	uint8_t amplitude;
	Channel chans[] = {CHANNEL_A, CHANNEL_B, CHANNEL_C};

	srand(time(NULL));
	ay38910_init(bc1_pin, bdir_pin, bus_port);

	for(int chan_idx = 0; chan_idx < sizeof chans; chan_idx++)
	{
		amplitude = (uint8_t)(rand() % 256); 
		ay38910_set_amplitude(chans[chan_idx], 0x1F & amplitude);
		TEST_ASSERT_EQUAL(port_mock_a, 0x00);
		TEST_ASSERT_EQUAL(port_mock_b, 0x1F & amplitude);
	}
}

void test_ay38910_set_envelope(void)
{
	uint16_t frequency;
	EnvelopeShape envs[] = {
		SHAPE_HOLD, SHAPE_ALTERNATE,
		SHAPE_ATTACK, SHAPE_CONTINUE
	};
	
	srand(time(NULL));
	ay38910_init(bc1_pin, bdir_pin, bus_port);

	for(int chan_idx = 0; chan_idx < sizeof envs; chan_idx++)
	{
		frequency = (uint16_t)rand();
		ay38910_play_note(envs[chan_idx], frequency);
		TEST_ASSERT_EQUAL(port_mock_a, 0x00);
	}
}

#endif // TEST
