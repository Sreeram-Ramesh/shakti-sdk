
The readme is a brief overview on how pwm based interrupt.

### OVERVIEW
  * This is the program tested different interrupt based on pwm.
  * src code:pwminterrupt_v2.c

### STEPS
  ##### The different type of interrupt are:
  - Rise Interrupt.
  - Fall Interrupt.
  - Half Period Interrupt.
  - Interrupt when Output is disable.

  ##### To get Rise Interrupt:
  - Make enable Rise Interrupt bits in the control register to get Rise Interrupt.
  - Uncomment the the Rise Interrupt section in the src code to get the Interrupt.

  ##### To get Fall Interrupt:
  - Make enable Fall Interrupt bits in the control register to get Fall Interrupt.
  - Uncomment the the Fall Interrupt section in the src code to get the Interrupt.

  ##### To get Half Period Interrupt:
  - Make enable Half Period Interrupt bits in the control register to get Half Period Interrupt.
  - Uncomment the the Half Period Interrupt section in the src code to get the Interrupt.

  ##### To get Rise Interrupt with O/P disable:  
  - Make enable Rise Interrupt bits and disable output bit in the control register to get Rise Interrupt.
  - Uncomment the the Rise Interrupt with O/P disable section in the src code to get the Interrupt.
    

### RESULTS
  * Set by the code, that board led will toggle when interrupt occurs. 
  * Only interrupt will occur which can be seen through console window.

## NOTE
  - All the interrupt in the src code checked through PWM_0 pins.
