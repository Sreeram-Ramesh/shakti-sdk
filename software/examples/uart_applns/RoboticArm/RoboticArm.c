#include "pwmv2.h"

#define PWM_0 0
#define PWM_1 1

#define PERIOD 15685

#define GET_DUTY_VAL(x) (int)(((1 + (float)(x*2 / 180)) / 20) * PERIOD)

// #define GET_DUTY_VAL(x) (int)((float)4.5*(x+270))




void delay_(int x) {
	while(x--);
}
void init_servo() {
	pwm_configure(PWM_0, PERIOD, GET_DUTY_VAL(0), no_interrupt, 0x1, false);
	pwm_set_control(PWM_0, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
	// pwm_set_control(PWM_0, (0x100f));
	pwm_start(PWM_0);
}

void set_servo(int angle) {
	pwm_set_duty_cycle(PWM_0,GET_DUTY_VAL(angle));
	pwm_set_control(PWM_0, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
	// delay_loop(1000,1000);
	// pwm_set_control(PWM_0, (0x100f));
}

int main()
{
	pwm_init();
	// pwm_clear(PWM_0);
	pwm_set_prescalar_value(PWM_0, 50);
	pwm_stop(PWM_0);
	init_servo();
	delay_(10000000);
	set_servo(90);
	delay_(10000000);
	set_servo(180);
}
