#ifndef AY3891_SYNTH_PINOUT_H
#define AY3891_SYNTH_PINOUT_H

#define pin(port,pin) port,pin

#define oc2a_pin     B,4
#define bdir_pin     D,1
#define bc1_pin      D,0
#define ay_bus_port  A

#define rs_pin       E,5
#define en_pin       E,4
#define lcd_bus      C
#define pwm_ch0      B,7

#endif //AY3891_SYNTH_PINOUT_H
