#include "pwmv2.h"
#include <stdio.h>

#define PWM_0 0

#define PERIOD 15685

#define GET_DUTY_VAL(x) (int)(((1 + (float)(x*2 / 180)) / 20) * PERIOD)

// #define GET_DUTY_VAL(x) (int)((float)4.5*(x+270))




void delay_(int x) {
	while(x--);
}
void init_servo() {
	pwm_configure(PWM_0, PERIOD, GET_DUTY_VAL(0), no_interrupt, 0x1, false);
	printf("\nInitialized to %d\n",GET_DUTY_VAL(0));
	pwm_set_control(PWM_0, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
	// pwm_set_control(PWM_0, (0x100f));
	pwm_start(PWM_0);
}

void rotate_motor(int angle1,int angle2){
	if(angle1<angle2) {
		int angle;
		for (angle = angle1; angle<=angle2+180; angle+=3) {
			pwm_set_duty_cycle(PWM_0,GET_DUTY_VAL(angle));
			printf("Duty_cycle %d\n",GET_DUTY_VAL(angle));
			pwm_set_control(PWM_0, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
			// pwm_set_control(PWM_0, (0x100f));
			// delay_loop(10,10);
			delay_(500);
		}
	}
	else {
		int angle;
		for (angle = angle1; angle>=angle2+180; angle-=3) {
			pwm_set_duty_cycle(PWM_0,GET_DUTY_VAL(angle));
			printf("Duty_cycle %d\n",GET_DUTY_VAL(angle));
			pwm_set_control(PWM_0, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
			// pwm_set_control(PWM_0, (0x100f));
			delay_(500);
			// delay_loop(10,10);
		}
	}
}

void set_servo(int angle) {
	pwm_set_duty_cycle(PWM_0,GET_DUTY_VAL(angle));
	printf("\nServo is set to the angle of %d degrees.\n",GET_DUTY_VAL(angle));
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
	delay_loop(1000,1000);
	while(1) {
		rotate_motor(0,180);
		// set_servo(0);
		delay(10000);
	}
}
