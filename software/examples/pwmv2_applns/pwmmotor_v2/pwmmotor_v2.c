#include "pwmv2.h"
#include <stdio.h>

#define PWM_0 0
#define PWM_1 1
#define PWM_2 2

#define PERIOD 15685

#define GET_DUTY_VAL(x) (int)(((1 + ((float)x*2 / 180)) / 20) * PERIOD)

// #define GET_DUTY_VAL(x) (int)((float)4.5*(x+270))




void delay_(int x) {
	while(x--);
}
void init_servo_0() {
	pwm_configure(PWM_0, PERIOD, GET_DUTY_VAL(0), no_interrupt, 0x1, false);
	printf("\nInitialized to %d\n",GET_DUTY_VAL(0));
	pwm_set_control(PWM_0, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
	// pwm_set_control(PWM_0, (0x100f));
	pwm_start(PWM_0);
}

void init_servo_1() {
	pwm_configure(PWM_1, PERIOD, GET_DUTY_VAL(0), no_interrupt, 0x1, false);
	printf("\nInitialized to %d\n",GET_DUTY_VAL(0));
	pwm_set_control(PWM_1, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
	// pwm_set_control(PWM_0, (0x100f));
	pwm_start(PWM_1);
}

void init_servo_2() {
	pwm_configure(PWM_2, PERIOD, GET_DUTY_VAL(0), no_interrupt, 0x1, false);
	printf("\nInitialized to %d\n",GET_DUTY_VAL(0));
	pwm_set_control(PWM_2, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
	// pwm_set_control(PWM_0, (0x100f));
	pwm_start(PWM_2);
}

void rotate_motor(int angle1,int angle2){
	set_servo(angle1);
	if(angle1<angle2) {
		int angle;
		for (angle = angle1; angle<=angle2+180; angle+=3) {
			//pwm_set_duty_cycle(PWM_0,GET_DUTY_VAL(angle));
			//printf("Duty_cycle %d\n",GET_DUTY_VAL(angle));
			//pwm_set_control(PWM_0, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
			// pwm_set_control(PWM_0, (0x100f));
			// delay_loop(10,10);
			set_servo(angle);
			delay_(500);
		}
	}
	else {
		int angle;
		for (angle = angle1; angle>=angle2+180; angle-=3) {
			//pwm_set_duty_cycle(PWM_0,GET_DUTY_VAL(angle));
			//printf("Duty_cycle %d\n",GET_DUTY_VAL(angle));
			//pwm_set_control(PWM_0, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
			// pwm_set_control(PWM_0, (0x100f));
			set_servo(angle);
			delay_(500);
			// delay_loop(10,10);
		}
	}
}

void set_servo(int servo, int angle) {
	pwm_set_duty_cycle(servo, GET_DUTY_VAL(angle));
	printf("\nServo is set to the angle of %d degrees.\n",GET_DUTY_VAL(angle));
	pwm_set_control(servo, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
	// delay_loop(1000,1000);
	// pwm_set_control(PWM_0, (0x100f));
}

int main()
{
	pwm_init();
	// pwm_clear(PWM_0);
	pwm_set_prescalar_value(PWM_0, 40);
	pwm_stop(PWM_0);
	pwm_set_prescalar_value(PWM_1, 40);
	pwm_stop(PWM_1);
	pwm_set_prescalar_value(PWM_2, 40);
	pwm_stop(PWM_2);
	
	init_servo_0();
	init_servo_1();
	init_servo_2();
	
	delay_(5000000);
	
	set_servo(PWM_2, 43);
	delay_(9000000);
	set_servo(PWM_1, 0);
	delay_(9000000);
	set_servo(PWM_0, 103);
	delay_(20000000);
	set_servo(PWM_0, 0);
	delay_(9000000);
	set_servo(PWM_1, 0);
	delay_(9000000);
	set_servo(PWM_2, 0);
	/*
	set_servo(PWM_2, 90);
	delay_(5000000);
	set_servo(PWM_1, 10);
	delay_(5000000);
	set_servo(PWM_0, 15);
	delay_(5000000);
	set_servo(PWM_0, 45);
	delay_(5000000);
	set_servo(PWM_1, 45);
	delay_(5000000);
	set_servo(PWM_2, 45);
	delay_(5000000);
	set_servo(PWM_0, 0);
	delay_(5000000);
	set_servo(PWM_1, 0);
	delay_(5000000);
	set_servo(PWM_2, 0);
	*/
	//rotate_motor(0,90);
}
