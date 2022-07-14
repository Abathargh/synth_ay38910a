#ifndef AY3891_SYNTH_PINOUT_H
#define AY3891_SYNTH_PINOUT_H

#define pin(port,pin) port,pin

#if defined(__AVR_ATmega2560__)

#define keyboard     F
#define ay_bus_port  A
#define oc2a_pin     B,4

#define rs_pin       E,5 
#define en_pin       E,4
#define lcd_bus      C
#define pwm_ch0      B,7

#elif defined(__AVR_ATmega644__)

#define keyboard     B
#define ay_bus_port  A
#define oc2a_pin     D,7

#define rs_pin       C,7
#define en_pin       C,6
#define lcd_bus      A
#define pwm_ch0      B,3

#endif

#define bdir_pin     C,7
#define bc1_pin      C,6

#define potentiom    F,0


#endif //AY3891_SYNTH_PINOUT_H
