/***************************************************************************
* Project           		: shakti devt board
* Name of the file	     	: pwmpolarity_v2.c
* Brief Description of file     : Control an led with the help of a button, gpio based.
* Name of Author    	        : Soutrick Roy Chowdhury
* Email ID                      : soutrickofficial@gmail.com

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
@file pwmpolarity_v2.c
@brief example file change the polariry on pwm pins
@detail It will change the polariry after a certain delay
*/

#include "pwmv2.h"
#include "pinmux.h"
#include <stdio.h>

#define PWM_0 0

/** @fn int main()
 * @brief main function that runs the code
 * @return zero
 */
int main()
{
	check_pwmv2();
	*pinmux_config_reg = 0x2aa80;
	pwm_init();

	pwm_set_prescalar_value(PWM_0, 50000);
	pwm_clear(PWM_0);
	pwm_stop(PWM_0);
	pwm_configure(PWM_0, 0xfff, 0xff, no_interrupt, 0x1, false);
	pwm_start(PWM_0);
    //Check by enable and disable polarity

	log_info("\n=========== With PWM_OUTPUT_POLARITY ===========");
	pwm_set_control(PWM_0, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_OUTPUT_POLARITY )); 
	pwm_show_values(PWM_0);

	delay_loop(10000,10000);

	log_info("\n\n=========== Without PWM_OUTPUT_POLARITY ===========");
	pwm_clr_control(PWM_0, PWM_OUTPUT_POLARITY); 
	pwm_show_values(PWM_0);

	delay_loop(10000,10000);
	pwm_clear(PWM_0);
}