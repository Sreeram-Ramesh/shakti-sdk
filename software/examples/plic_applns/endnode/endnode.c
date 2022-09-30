/***************************************************************************
 * Project           			:  shakti devt board
 * Name of the file	     		:  ZigbeeLM75.c
 * Created date			        :  31.03.2022
 * Brief Description of file    :  Transmits data from UART1 - UART2 & vice versa.
 * Name of Student    	        :  Kolan Nikshith Reddy
 * Email id of student			:  cs20m030@smail.iitm.ac.in
 * Name of Mentor               :  Kottesswaran Ekambaram
 * Email id of mentor			:  kottee.1@gmail.com

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

#include "i2c.h"
#include <stdint.h>
#include <stddef.h>
#include "log.h"
#include "uart.h"
#include "pinmux.h"
#include "traps.h"
#include "platform.h"
#include "plic_driver.h"
#include <stdio.h>
#include <string.h>
// #include "xadc_driver.h"
#include <math.h>


#define I2C i2c_instance[1]

#define LM75_SLAVE_ADDRESS 0x90//Defines the Starting address of slave//
#define LM75_TEMP_REG_OFFSET 0x00
#define LM75_CONF_REG_OFFSET 0x01
#define LM75_HYST_REG_OFFSET 0x02
#define LM75_OFFSET_REG_OFFSET 0x03

#define DELAY_VALUE 900
#define PRESCALER_COUNT 0x1F
#define SCLK_COUNT 0x91

#define FIXEDOFFSET 14
#define MQ2 0
#define MQ7 1
#define LM75 2
enum location{start,lenbyte1,lenbyte0,frametype, frameid,addrbase, addr16bit0=13, addr16bit1, broadcast, options, rfdatabase};



unsigned int uart0_prev_rcv_isr_count_flag = 0;
unsigned int uart1_prev_rcv_isr_count_flag = 0;
unsigned int uart2_prev_rcv_isr_count_flag = 0;

unsigned int uart0_prev_tx_isr_count_flag = 0;
unsigned int uart1_prev_tx_isr_count_flag = 0;
unsigned int uart2_prev_tx_isr_count_flag = 0;
#if 1
	uint8_t uart1_buf[] = {"abcdefghijklmnopqrstuvwxyz"};
	uint8_t uart2_buf[] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
#else
	uint8_t uart1_buf[] = {"abcdef"};
	uint8_t uart2_buf[] = {"ABCDEF"};
#endif


#define UART_INST uart_instance[2]
#define USE_INTERRUPT
#define MAX_OUTPUT_SIZE 1024
//#define APIPOST "https://google.com/abc" 
#define APIPOST "http://142.250.67.46/" 

#define DATA "Hello_World"
#define TIME "1000"

void init_variables()
{
	uart0_complete = 0;
	uart1_complete = 0;
	uart2_complete = 0;
	uart0_tx_isr_count = 0;
	uart0_rcv_isr_count = 0;
	uart1_tx_isr_count = 0;
	uart1_rcv_isr_count = 0;
	uart2_tx_isr_count = 0;
	uart2_rcv_isr_count = 0;
	memset(u0_rcv_char,'\0', UARTX_BUFFER_SIZE);
	memset(u1_rcv_char,'\0', UARTX_BUFFER_SIZE);
	memset(u2_rcv_char,'\0', UARTX_BUFFER_SIZE);
	uart0_prev_rcv_isr_count_flag = 0;
	uart1_prev_rcv_isr_count_flag = 0;
	uart2_prev_rcv_isr_count_flag = 0;

	uart0_prev_tx_isr_count_flag = 0;
	uart1_prev_tx_isr_count_flag = 0;
	uart2_prev_tx_isr_count_flag = 0;
}
extern volatile unsigned int* pinmux_config_reg;

void free_uart_buffers()
{
	flush_uart( uart_instance[0] );
	flush_uart( uart_instance[1] );
	flush_uart( uart_instance[2] );

}

int SendDataRF(uart_struct * uart_instance, char *rfdata){

   //Creating the RF Frame to send data to end device
    unsigned char data[500];//= {'\x7E','\x00' ,'\x16','\x10','\x01','\x00','\x13','\xA2','\x00','\x41','\xDE','\x54','\x0F','\xFF','\xFE','\x00','\x00','\x66','\x72','\x6F','\x6D','\x63','\x6F','\x6F','\x64','\x61'};
	//printf("size of send: %d\n");
    //start delimiter  
	memset(data, NULL, sizeof(data));  
    data[start] = '\x7E';

    //rf data
   //char rfdata[100] = "fromcood";
    for(int i=0;i<strlen(rfdata);i++){
        data[rfdatabase+i] = rfdata[i]; 
    }

    //length 2 bytes
    unsigned short int len = FIXEDOFFSET + strlen(rfdata);
    char *p = (char*)&len;
    data[lenbyte0] = *(p++); // length (2B)
    data[lenbyte1] = *p;

    //frametype and frame id
    data[frametype] = '\x10';
    data[frameid] = '\x01';

    //destination address 64-bit
    char addr[8] ={ '\x00','\x00','\x00','\x00','\x00','\x00','\x00','\x00'};
   
    for(int i=0;i<8;i++){
        data[addrbase+i] = addr[i]; 
    }
    //destination address 16-bit
    data[addr16bit0] = '\xFF';
    data[addr16bit1] = '\xFE';
    data[broadcast] = '\x00';
    data[options] = '\x00';

    //checksum calculation
    unsigned int sum=0;
    for(int i=frametype;i<rfdatabase+strlen(rfdata);i++){
         sum+=data[i];
    }
    int checksum = 0xFF - (sum&0xFF);
    int checksumoffest = rfdatabase+strlen(rfdata);
    data[checksumoffest] = checksum;

    printf("sending rf data\n");

    // for(int i=start;i<rfdatabase+strlen(rfdata)+1;i++){
    //     printf("%d %x\n", i, data[i]);
    // }

    //sending RF data
	for (int i = 0; i < len+4; i++)
	{	//printf("sending0: %d:%d:%c:%x\n",i,  data[i], data[i], data[i]);
		write_uart_character(uart_instance,data[i]);
	}
	
//    write_uart_string(uart_instance, data);   

}

int calchecksum(char *data){
    //checksum calculation
    unsigned int sum=0;
    for(int i=frametype;i<strlen(data)-1;i++){
         sum+=data[i];
    }
    if(sum&0xFF == 0xFF){
        return 1;
    }
    else return 0;
}


/** @fn int read_lm75_register(i2c_struct * i2c_instance, unsigned int reg_offset, unsigned int *readTemp, unsigned long delay)
 * @brief Read the lm75 register.
 * @details Reads the value of the passed register of LM75 over I2C Interface. 
 * @param i2c_struct*
 * @param reg_offset
 * @param *readTemp
 * @param delay
 * @return  Read Status (Zero on Success).
 */
int read_lm75_register(i2c_struct * i2c_instance, unsigned int reg_offset, unsigned int *readTemp, unsigned long delay)
{
	unsigned char read_buf[4] = {'\0'};
	int i = 0, j = 0,  k = 0, status=0;
	unsigned char temp = 0;
//Writes the slave address for write
	i2c_send_slave_address(i2c_instance, LM75_SLAVE_ADDRESS, I2C_WRITE, 800);
//Writes the pointer to address that needs to be read
	i2c_write_data(i2c_instance, reg_offset , delay);
//Stops the I2C transaction to start reading the temperature value.
	i2c_instance->control = I2C_STOP;


//Writes the slave address for read
	i2c_send_slave_address(i2c_instance, LM75_SLAVE_ADDRESS, I2C_READ, 800);


/* Make a dummy read as per spec of the I2C controller */
	i2c_read_data(i2c_instance, &temp, delay);

//Reads the MSB Byte of temperature [D9 - D1]
	i2c_read_data(i2c_instance, &read_buf[1], delay);
	i2c_instance->control = I2C_NACK;

//Reads the MSB Byte of temperature [D9 - D1]
	i2c_read_data(i2c_instance, &read_buf[0], delay);
	i2c_instance->control = I2C_STOP;
	*readTemp = (read_buf[1] << 1) | (read_buf[0] >> 7);

	return 0;
}

/** @fn int write_lm75_register(i2c_struct * i2c_instance, unsigned int reg_offset, unsigned int write_value, unsigned long delay)
 * @brief Writes the value into passed lm75 register 
 * @details Writes the passed value into the passed lm75 register address over I2C interface.
 * @param i2c_struct*
 * @param reg_offset
 * @param write_value
 * @param delay
 * @return Write Status (Zero on Success)
 */
int write_lm75_register(i2c_struct * i2c_instance, unsigned int reg_offset, unsigned int write_value, unsigned long delay)
{
	int i = 0, j = 0,  k = 0, status=0;
	unsigned int temp = 0;
	i2c_send_slave_address(i2c_instance, LM75_SLAVE_ADDRESS, I2C_WRITE, delay);
	i2c_write_data(i2c_instance, reg_offset /*LM75_TEMP_REG_OFFSET */, delay);
	i2c_write_data(i2c_instance,  ( (write_value >> 1) & 0xff) /*LM75_TEMP_REG_OFFSET */, delay);
	printf("\n Write Value[0]: %02x", (write_value >> 1) & 0xff);
	i2c_write_data(i2c_instance,  ( ( (write_value & 0x01) << 7) & 0xff) /*LM75_TEMP_REG_OFFSET */, delay);
	printf("\n Write Value[1]: %02x", ( ( (write_value & 0x01) << 7) & 0xff));
//Stops the I2C transaction to start reading the temperature value.
	i2c_instance->control = I2C_STOP;
	return 0;
}




int main(void){
printf("starting enddevice\n");
	unsigned int int_id = 1;
	register uint32_t retval;
	int i;
//	printf("\tUart 1 - Uart 2 Interrupt Test\n");

	init_variables();

	pinmux_enable_uart1_uart2();
	set_baud_rate(uart_instance[0], 115200);
	set_baud_rate(uart_instance[1], 9600);
	//set_baud_rate(uart_instance[2], 115200);

	plic_init();

	configure_interrupt(PLIC_INTERRUPT_26);
	isr_table[PLIC_INTERRUPT_26] = uart1_isr;
	// configure_interrupt(PLIC_INTERRUPT_27);
	// isr_table[PLIC_INTERRUPT_27] = uart2_isr;

	uart1_prev_tx_isr_count_flag = uart1_tx_isr_count;
	uart1_prev_rcv_isr_count_flag = uart1_rcv_isr_count;
	uart2_prev_tx_isr_count_flag = uart2_tx_isr_count;
	uart2_prev_rcv_isr_count_flag = uart2_rcv_isr_count;
	free_uart_buffers();
	printf("Before Tx00\n");
	printf("\n UART 1 Status Register: %x", uart_instance[1]->status);
	//printf("\n UART 2 Status Register: %x", uart_instance[2]->status);
	enable_uart_interrupts(uart_instance[1],  ENABLE_RX_NOT_EMPTY );//| ENABLE_RX_FULL);
	// enable_uart_interrupts(uart_instance[2],  ENABLE_RX_NOT_EMPTY );//| ENABLE_RX_FULL);

	// Enable Global (PLIC) interrupts.
	asm volatile("li      t0, 8\t\n"
		     "csrrs   zero, mstatus, t0\t\n"
		    );

	asm volatile("li      t0, 0x800\t\n"
		     "csrrs   zero, mie, t0\t\n"
		    );

	asm volatile(
		     "csrr %[retval], mstatus\n"
		     :
		     [retval]
		     "=r"
		     (retval)
		    );

	printf(" retval = %x\n", retval);
	asm volatile(
		     "csrr %[retval], mie\n"
		     :
		     [retval]
		     "=r"
		     (retval)
		    );

	printf(" retval = %x\n", retval);

	asm volatile(
		     "csrr %[retval], mip\n"
		     :
		     [retval]
		     "=r"
		     (retval)
		    );
	printf(" retval = %x\n", retval);
	
	int timeout;
	unsigned int tempReadValue = 0;
	unsigned long delay = 1000;
//	set_baud_rate(uart_instance[0], 115200);
    
	log_debug("\n\tI2C: LM75 Temperature Sensor I2C read\n");
    i2c_init();
	//Initialises I2C Controller
		if(config_i2c(I2C, PRESCALER_COUNT,SCLK_COUNT))
		{
				log_error("\tSomething Wrong In Initialization\n");
				return -1;
		}
	else
				log_info("\tIntilization Happened Fine\n");

	#ifdef LM75_HYST_CHECK
		write_lm75_register(I2C, LM75_HYST_REG_OFFSET, 0x30, delay);
			if(0 == read_lm75_register(I2C, LM75_HYST_REG_OFFSET, &tempReadValue, delay))
			{
				//Display Temperature value
	//			printf("\nTemperature read value is %x", tempReadValue);
	//				tempReadValue = ( (~tempReadValue) + 1 );
					log_info("\n\t Hyst Reg Value:=%u",tempReadValue);
			}
			else
			{
		//Display the error
				log_error("\nHyst Reg Value failed.");
			}
	#endif
	#ifdef LM75_OFFSET_CHECK
		write_lm75_register(I2C, LM75_OFFSET_REG_OFFSET, 0x35, delay);
		if(0 == read_lm75_register(I2C, LM75_OFFSET_REG_OFFSET, &tempReadValue, delay))
		{
			//Display Temperature value
				log_info("\n\t Offset Reg Value:=%u",tempReadValue);
		}
		else
		{
	//Display the error
			log_error("\nOffset Reg Value failed.");
		}
	#endif

   int j=0;
   int k=0;
	while(1)
	{
		for(int tt=0;tt<10;tt++){
			delay_loop(2000,1000);	
		}
		if(uart1_rcv_isr_count != 0)
			{
				u1_rcv_char[++uart1_rcv_isr_count] = '\0';
					log_info("\n UART 1 recv. Chars: %d:%d:\n\n", sizeof(u1_rcv_char), uart1_rcv_isr_count);

				for(int i=0;i<uart1_rcv_isr_count-1;i++){
					if(u1_rcv_char[i]  ==0x7E && i+3 < uart1_rcv_isr_count-1 &&  u1_rcv_char[i+3]==0x8B){
						log_info("Received acknowledgement from the receiver\n");
						i=i+10;
					}

					if(u1_rcv_char[i]==0x7E && i+3 < uart1_rcv_isr_count-1 &&  u1_rcv_char[i+3]==0x90){
						//log_info("Received data:\n");
						
						unsigned short int datalen;// = FIXEDOFFSET + strlen(rfdata);
						char *p = (char*)&datalen;
						p[0] = u1_rcv_char[i+2];
						p[1] = u1_rcv_char[i+1];

						char recvdata[500]={0};
						for(int tempi = 0;tempi<datalen-12;tempi++){
							recvdata[tempi] = u1_rcv_char[i+tempi+15]; 
						}
						log_info("datarcv::%s\n", recvdata);

						unsigned int datasum=0;
						int startchecksum = i+frametype, endchecksum = i+datalen+frametype;
						for(int tempi=startchecksum;tempi<=endchecksum;tempi++){
							datasum+=u1_rcv_char[tempi];
						}
						if(datasum&0xFF == 0xFF){
							log_info("checksum verified for received  data\n\n");
						}

						i=i+endchecksum;
					}
					//log_info("\n\t%d=%d:%c:%x", i, u1_rcv_char[i],u1_rcv_char[i],u1_rcv_char[i]);
				}
				uart1_rcv_isr_count = 0;
				memset(u1_rcv_char,'\0', UARTX_BUFFER_SIZE);
				
			}
			k++;
			k=k%3;

			if(k%3==0){
					char mq2values[200];
					if(0 == read_lm75_register(I2C, LM75_TEMP_REG_OFFSET, &tempReadValue, delay))
					{
						if(tempReadValue & 0x100) {
							tempReadValue = ( (~tempReadValue) + 1 ) >> 1;
							sprintf(mq2values,"2Current Temperature:=-%u°C",tempReadValue);
							// log_info("\n\t 2Current Temperature:=-%u°C",tempReadValue);
						}
						else {
							tempReadValue = tempReadValue >> 1;
							sprintf(mq2values,"2Current Temperature:=+%u°C",tempReadValue);
							// log_info("\n\t Current Temperature:=+%u°C",tempReadValue);
						}
					}
					else
					{
						sprintf(mq2values,"2Temperature read failed.");
					}					
					SendDataRF(uart_instance[1],mq2values);

			}
		delay_loop(2000,1000);
	
	};


	return 0;
}




