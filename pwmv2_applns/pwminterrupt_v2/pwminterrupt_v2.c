/***************************************************************************
* Project           		: shakti devt board
* Name of the file	     	: pwminterrupt_v2.c
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
@file pwminterrupt_v2.c
@brief example file to make the different pwminterrupt 
@detail It will make the interrupt after a certain frequency
*/

#include "pwmv2.h"
#include "plic_driver.h"
#include "gpiov2.h"
#define PWM_0 0

void handle_pwm_interrupt( uint32_t num)
{
	gpiov2_init();
	gpiov2_instance->direction = 0x10000;
	gpiov2_instance->toggle = 0x10000;

    pwm_set_control((6-num),0x80);
	log_info("pwm interrupt handled\n");
}

int main()
{

	printf("\n====================================================\n");

    register unsigned int retval;
	int i;

	plic_init();

	/*
	   Configure pwm module interrupt
	*/
	configure_interrupt(PLIC_INTERRUPT_6);

	/* For checking PWM Interrupt by toggling one board LED */
	// isr_table[PLIC_INTERRUPT_6] = handle_pwm_interrupt;

	/* For checking all the test case*/
	isr_table[PLIC_INTERRUPT_6] = pwm_isr_handler0;


	/**we need to set the period, duty cycle and the clock divisor in order
	 *to set it to the frequency required. Base clock is 50MHz
	 **/
	asm volatile("li      t0, 8\t\n"
			"csrrs   zero, mstatus, t0\t\n"
		    );

	asm volatile("li      t0, 0x800\t\n"
			"csrrs   zero, mie, t0\t\n"
		    );

	asm volatile("csrr %[retval], mstatus\n"
			:
			[retval]
			"=r"
			(retval)
		    );

	log_info("mstatus = %x\n", retval);
	asm volatile("csrr %[retval], mie\n"
			:
			[retval]
			"=r"
			(retval)
		    );

	log_info("mie = %x\n", retval);

	asm volatile("csrr %[retval], mip\n"
			:
			[retval]
			"=r"
			(retval)
		    );

	log_info("mip = %x\n", retval);


    check_pwmv2();
	/* Enable when need rise interrupt */
#if 1	
	pwm_init();
    pwm_set_prescalar_value(PWM_0, 0xf2);
 	pwm_configure(PWM_0, 0xff, 0x01, rise_interrupt, 0x1, false);
    pwm_start(PWM_0);
	pwm_show_values(PWM_0);
    pwm_set_control(PWM_0, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
#endif

	/* Enable when fall interrupt */
#if 0
	pwm_init();
    pwm_set_prescalar_value(PWM_0, 0xf2);
 	pwm_configure(PWM_0, 0xff, 80, fall_interrupt, 0x1, false);
    pwm_start(PWM_0);
	pwm_show_values(PWM_0);
    pwm_set_control(PWM_0, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_FALL_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
#endif

	/* Enable when halfperiod_interrupt  */
#if 0
	pwm_init();
    pwm_set_prescalar_value(PWM_0, 0xf2);
 	pwm_configure(PWM_0, 0xff, 80, halfperiod_interrupt, 0x1, false);
    pwm_start(PWM_0);
	pwm_show_values(PWM_0);
    pwm_set_control(PWM_0, (PWM_ENABLE | PWM_UPDATE_ENABLE |PWM_OUTPUT_ENABLE | PWM_HALFPERIOD_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
#endif

	/* Uncomment for checking Output Disable with PWM Interrupt */
#if 0
	pwm_init();
    pwm_set_prescalar_value(PWM_0, 0xf2);
 	pwm_configure(PWM_0, 0xff, 0x01, rise_interrupt, 0x1, false);
    pwm_start(PWM_0);
	pwm_show_values(PWM_0);
    pwm_set_control(PWM_0, (PWM_ENABLE | PWM_UPDATE_ENABLE | PWM_RISE_INTERRUPT_ENABLE | PWM_OUTPUT_POLARITY ));
#endif

	while(1);
    return 0;
}