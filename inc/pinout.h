#ifndef AY3891_SYNTH_PINOUT_H
#define AY3891_SYNTH_PINOUT_H

#define pin(port,pin) port,pin

#if defined(__AVR_ATmega2560__)

#define keyboard     F
#define ay_bus_port  A
#define bdir_pin     C,7
#define bc1_pin      C,6
#define oc2a_pin     B,4

#define rs_pin       E,5 
#define en_pin       E,4
#define lcd_bus      C
#define pwm_ch0      B,7

#elif defined(__AVR_ATmega644__)

/* Keyboard matrix connections */
#define keyboard_in    A   // A[0:2]
#define keyboard_out   D

/* Ay38910a connections */
#define ay_bus_port    B
#define bdir_pin       C,7
#define bc1_pin        C,6
#define oc2a_pin       D,7

/* LCD screen connections*/
#define rs_pin         C,1
#define en_pin         C,0
#define lcd_bus        A   // A[4:7]
#define pwm_ch0        D,5

/* Navigation inputs connections */

#define browse         D,4
#define confirm        D,6
#define potentiometer  A,3

#endif



#endif //AY3891_SYNTH_PINOUT_H
