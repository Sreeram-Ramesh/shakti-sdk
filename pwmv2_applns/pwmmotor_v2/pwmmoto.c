#include "pwmv2.h"
#include "pinmux.h"
#include <stdio.h>

#define PWM_0 0


#define MIN_DUTY 0.5
#define MAX_DUTY 2.5
//#define GET_DUTY_VAL(x) (int)((0.5 + ((2)*(float)x/180.0))*1275)
#define GET_DUTY_VAL(x) (int) (1174+(x*8.21111))
// float raw_value = 4.344 * (deg + 270);
// pwm_duty_reg = (raw_value < 391) ? 391 : (raw_value > 1954) ? 1954 : raw_value;

void init_servo() {
	pwm_configure(PWM_0, 15650, 1174, no_interrupt, 0x1, false);	//0xf0 0x80
	pwm_start(PWM_0);
	//delay(1); //wait for servo motor to come to zero degree.
}


void rotate_motor(int angle1,int angle2){
	if(angle1<angle2) {
		int angle;
		for (angle = angle1; angle<=angle2;angle+=3) {
			pwm_set_duty_cycle(PWM_0,GET_DUTY_VAL(angle));
			pwm_set_control(PWM_0, 0x100F);
		}
	}
	else {
		int angle;
		for (angle = angle1; angle>=angle2;angle-=3) {
			pwm_set_duty_cycle(PWM_0,GET_DUTY_VAL(angle));
			pwm_set_control(PWM_0, 0x100F);
		}
	}
}

int main()
{
	//check_pwmv2();
	pwm_init();
	//*pinmux_config_reg = 0x2aa80;
	pwm_clear(PWM_0);
	pwm_set_prescalar_value(PWM_0, 50);
	pwm_stop(PWM_0);	
	while(1) {
	pwm_configure(PWM_0, 15630, 1913 , no_interrupt, 0x1, false);	
	// 0.5ms-391,1ms-781 ,1.5ms-1172 ,2ms-1563, 2.5ms-1954          90deg- 391 diff   1deg - 4.344        (4.344*(deg+270))
		// -180 - 0.5ms, -90 - 1ms, 0-1.5ms, +90 - 2ms, +180 - 2.5 ms												max-1954 min-391		
	pwm_start(PWM_0);
	pwm_set_control(PWM_0, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
	delay(10);
	rotate_motor(0,90);
	//pwm_set_duty_cycle(PWM_0, 1174);
	//pwm_set_control(PWM_0, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
	delay(10);
	//pwm_clear(PWM_0);
	}
}
