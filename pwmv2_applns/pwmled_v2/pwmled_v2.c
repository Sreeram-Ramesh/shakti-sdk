/***************************************************************************
* Project               	        	:  shakti devt board
* Name of the file	            	    :  pwmled_v2.c
* Brief Description of file             :  example file for pwm led.
* Name of Author    	                :  Soutrick Roy Chowdhury
* Email ID                              :  soutrickofficial@gmail.com
 Copyright (C) 2019  IIT Madras. All rights reserved.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.

***************************************************************************/
/**
@file pwmled.c
@brief example file to make the led blink at specified frequency
@detail it runs the led and causes it to blink at specified frequency using the pwm module
*/

#include "pwmv2.h"
#include "pinmux.h"
#include <stdio.h>

#define PWM_0 0
#define PWM_1 1 
#define PWM_2 2 
#define PWM_3 3
#define PWM_4 4 
#define PWM_5 2

/** @fn int main()
 * @brief main function that runs the code
 * @return zero
 */
int main()
{
	check_pwmv2();
	//*pinmux_config_reg = 0x2aa80;
	pwm_init();

	//Uncomment this for testing PWM 0
	pwm_set_prescalar_value(PWM_0, 5000);
	pwm_clear(PWM_0);
	pwm_stop(PWM_0);
	pwm_configure(PWM_0, 10000, 5000, no_interrupt, 0x1, false);
	pwm_start(PWM_0);
	pwm_set_control(PWM_0, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_OUTPUT_POLARITY ));
	pwm_show_values(PWM_0);
	pwm_show_frequency(PWM_0, 5000, 10000);

	//Uncomment this for testing PWM 1
	pwm_set_prescalar_value(PWM_1, 5000);
	pwm_clear(PWM_1);
	pwm_stop(PWM_1);
	pwm_configure(PWM_1, 10000, 5000, no_interrupt, 0x1, false);
	pwm_start(PWM_1);
	pwm_set_control(PWM_1, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
	pwm_show_values(PWM_1);
	pwm_show_frequency(PWM_1, 5000, 10000);

	//Uncomment this for testing PWM 2
	pwm_set_prescalar_value(PWM_2, 5000);
	pwm_clear(PWM_2);
	pwm_stop(PWM_2);
	pwm_configure(PWM_2, 10000, 5000, no_interrupt, 0x1, false);
	pwm_start(PWM_2);
	pwm_set_control(PWM_2, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
	pwm_show_values(PWM_2);
	pwm_show_frequency(PWM_2, 5000, 10000);

	//Uncomment this for testing PWM 3
	pwm_set_prescalar_value(PWM_3, 5000);
	pwm_clear(PWM_3);
	pwm_stop(PWM_3);
	pwm_configure(PWM_3, 10000, 5000, no_interrupt, 0x1, false);
	pwm_start(PWM_3);
	pwm_set_control(PWM_3, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
	pwm_show_values(PWM_3);
	pwm_show_frequency(PWM_3, 5000, 10000);


	//Uncomment this for testing PWM 4
	pwm_set_prescalar_value(PWM_4, 5000);
	pwm_clear(PWM_4);
	pwm_stop(PWM_4);
	pwm_configure(PWM_4, 10000, 5000, no_interrupt, 0x1, false);
	pwm_start(PWM_4);
	pwm_set_control(PWM_4, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
	pwm_show_values(PWM_4);
	pwm_show_frequency(PWM_3, 5000, 10000);


	//Uncomment this for testing PWM 5
	pwm_set_prescalar_value(PWM_5, 5000);
	pwm_clear(PWM_5);
	pwm_stop(PWM_5);
	pwm_configure(PWM_5, 10000, 5000, no_interrupt, 0x1, false);
	pwm_start(PWM_5);
	pwm_set_control(PWM_5, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
	pwm_show_values(PWM_5);
	pwm_show_frequency(PWM_5, 5000, 10000);



	//Uncomment this for testing PWM 0 as disabling output 
	pwm_set_prescalar_value(PWM_0, 50000);
	pwm_clear(PWM_0);
	pwm_stop(PWM_0);
	pwm_configure(PWM_0, 0xf0, 0x80, no_interrupt, 0x1, false);
	pwm_start(PWM_0);
	pwm_set_control(PWM_0, (PWM_ENABLE | PWM_UPDATE_ENABLE | PWM_OUTPUT_POLARITY ));
	pwm_show_values(PWM_0);
	pwm_show_frequency(PWM_0, 50000, 0xf0);
	return 0;
}
