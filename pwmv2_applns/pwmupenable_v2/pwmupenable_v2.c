/***************************************************************************
* Project           		: shakti devt board
* Name of the file	     	: pwmupenable_v2.c
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
@file pwmupenable_v2.c
@brief example file Update the control register after certain delay
@detail It will help to update the control reg after a certain delay
*/


/* LED WILL GLOW ONLY ONCE */
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

	//Uncomment this for testing PWM 0
	pwm_set_prescalar_value(PWM_0, 5000);
	pwm_clear(PWM_0);
	pwm_stop(PWM_0);

	log_info("\n=========== Setting a particular period and duty ===========");
	pwm_configure(PWM_0, 10000, 5000, no_interrupt, 0x1, false);
	pwm_start(PWM_0);
	pwm_set_control(PWM_0, (PWM_ENABLE| PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_OUTPUT_POLARITY ));
	pwm_show_values(PWM_0);
	pwm_show_frequency(PWM_0, 5000, 10000);

	delay_loop(5000, 5000);

	/* Disabling PWM_UPBATE_ENABLE bits*/
	pwm_clr_control(PWM_0, PWM_UPDATE_ENABLE);

	log_info("\n=========== Updating period and duty ===========");
	pwm_configure(PWM_0, 5000, 2500, no_interrupt, 0x1, false);
	pwm_set_control(PWM_0, (PWM_ENABLE| PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_OUTPUT_POLARITY ));
	pwm_show_values(PWM_0);
	pwm_show_frequency(PWM_0, 5000, 10000);
    return 0;
}