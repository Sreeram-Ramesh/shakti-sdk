/***************************************************************************
 * Project           			:  shakti devt board
 * Name of the file	     		:  uart1_uart2_interrupt.c
 * Created date			        :  31.03.2022
 * Brief Description of file             :  Transmits data from UART1 - UART2 & vice versa.
 * Name of Author    	                :  E. Kotteeswaran
 * Email ID                              :  kottee.1@gmail.com

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


// #define UART_INST uart_instance[2]
#define USE_INTERRUPT
#define MAX_OUTPUT_SIZE 1024
//#define APIPOST "https://google.com/abc" 
// #define APIPOST "http://15.207.222.76/admin/data/1" 

// #define DATA "Hello_World"
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

int str_length(char *str)
{
    int len=0;
    while(*str!='\0')
    {
        len++;
        str++;
    }
    return len;
}

int int_to_str(int data, char * str)
{
   int i=0;
   while( data > 0 )
   {
      str[i]='0'+ (data%10);
      data/=10;
      i++;
   }
   str[i]='\0';
   int n=str_length(str)-1;
   int len=n+1;
   i=0;
   while(i<n)
   {
      char ch=str[i];
      str[i]=str[n];
      str[n]=ch;
      i++;
      n--;
   }
   return len;
}

int write_cmd(uart_struct * uart_instance, char *str)
{
    while(*str!='\0')
    {
       write_uart_character(uart_instance,*str);
       str++;
    }
    write_uart_character(uart_instance,'\r');
}

int cmp_str(char *str1,char *str2)
{
   while(*str1!='\0' && *str2!='\0')
   {
      if(*str1==*str2)
      {
         str1++;
         str2++;
      }
      else
         return 0;
   }
   return 1;
}

int read_output(uart_struct * uart_instance, char *st)
{
   delay(1);
   u2_rcv_char[++uart2_rcv_isr_count] = '\0';
   if(strstr(u2_rcv_char,st)==NULL)
   {
      uart2_rcv_isr_count = 0;
	   memset(u2_rcv_char,'\0', UARTX_BUFFER_SIZE);
      return 0;
   }
   uart2_rcv_isr_count = 0;
	memset(u2_rcv_char,'\0', UARTX_BUFFER_SIZE);
   return 1;
}






int SendDataRF(uart_struct * uart_instance, int enddevice, char *rfdata){

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
    char addr[4][8] ={ {'\x00','\x13','\xA2','\x00','\x41','\xF1','\xD3','\xD7'},
                       {'\x00','\x13','\xA2','\x00','\x41','\xF1','\xD3','\xD7'},
                       {'\x00','\x13','\xA2','\x00','\x41','\xF1','\xD3','\xD7'},
                       {'\x00','\x13','\xA2','\x00','\x41','\xF1','\xD3','\xD7'} 
                     };
   
    for(int i=0;i<8;i++){
        data[addrbase+i] = addr[enddevice][i]; 
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

   if(enddevice == MQ2){
          printf("\nsending response to MQ2 node\n");
   }
   else if(enddevice ==  MQ7){
          printf("\nsending remote server response to MQ7 node\n");
   }
   else if(enddevice ==  LM75){
          printf("\nsending remote server response to LM75 node\n");
   }


   //  printf("sending rf data\n");

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

void checkReceivedData(char *recv){
   int ch = recv[0]-'0';
   switch (ch)
      {
      case MQ2: 
         printf("\nreceived data from MQ2 node:\n\t\"");
         break;
      
      case MQ7:
         printf("\nreceived data from MQ7 node:\n\t\"");
         break;

      case LM75:
         printf("\nreceived data from LM75 node:\n\t\"");
         break;

      default:
         break;
      }
   printf("%s\"\n", recv+1);
}

void sendDatatoNode(char recv){
      int choice = recv-'0';
      switch (choice)
      {
      case MQ2: 
         SendDataRF(uart_instance[1], MQ2 , "success. data sent ");
         break;
      
      case MQ7:
         SendDataRF(uart_instance[1], MQ7 , "success. data sent ");
         break;

      case LM75:
         SendDataRF(uart_instance[1], LM75 , "success. data sent ");
         break;

      default:
         break;
      }
}




int main(void){
	unsigned int int_id = 1;
	register uint32_t retval;
	int i;
//	printf("\tUart 1 - Uart 2 Interrupt Test\n");

	init_variables();

	pinmux_enable_uart1_uart2();
	set_baud_rate(uart_instance[0], 115200);
	set_baud_rate(uart_instance[1], 9600);
	// set_baud_rate(uart_instance[2], 115200);

	plic_init();

	configure_interrupt(PLIC_INTERRUPT_26);
	isr_table[PLIC_INTERRUPT_26] = uart1_isr;
	// configure_interrupt(PLIC_INTERRUPT_27);
	// isr_table[PLIC_INTERRUPT_27] = uart2_isr;

	uart1_prev_tx_isr_count_flag = uart1_tx_isr_count;
	uart1_prev_rcv_isr_count_flag = uart1_rcv_isr_count;
	// uart2_prev_tx_isr_count_flag = uart2_tx_isr_count;
	// uart2_prev_rcv_isr_count_flag = uart2_rcv_isr_count;
	free_uart_buffers();
   printf("\nstarting Co-ordinator\n");
	printf("\nBefore Tx00\n");
	printf("\n UART 1 Status Register: %x", uart_instance[1]->status);
	// printf("\n UART 2 Status Register: %x", uart_instance[2]->status);
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
	
   //    if(setup(uart_instance[2])){
	//    printf("\nLTE module initialised!");
   // }
   // else{
	//    printf("\nLTE module not working!");
   // }

   int j=0;
   int k=0;
	while(1)
	{
		
		
		//write_uart_string(uart_instance[1],"mnopqrstuvwx");
		for(int tt=0;tt<10;tt++){
			delay_loop(2000,1000);	
		}
		if(uart1_rcv_isr_count != 0)
			{
				u1_rcv_char[++uart1_rcv_isr_count] = '\0';
					// log_info("\n UART 1 recv. Chars: %d:%d:\n\n", sizeof(u1_rcv_char), uart1_rcv_isr_count);

				for(int i=0;i<uart1_rcv_isr_count-1;i++){
					if(u1_rcv_char[i]  ==0x7E && i+3 < uart1_rcv_isr_count-1 &&  u1_rcv_char[i+3]==0x8B){
						log_info("Received acknowledgement packet\n");
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
//						log_info("datarcv::%s\n", recvdata);
                  checkReceivedData(recvdata);

						unsigned int datasum=0;
						int startchecksum = i+frametype, endchecksum = i+datalen+frametype;
						for(int tempi=startchecksum;tempi<=endchecksum;tempi++){
							datasum+=u1_rcv_char[tempi];
						}
						if(datasum&0xFF == 0xFF){
							log_info("checksum verified for received  data\n\n");
						}
                  // SendDataPOST(uart_instance[2],APIPOST,recvdata,TIME);

                  sendDatatoNode(recvdata[0]);

						i=i+endchecksum;
					}
					//log_info("\n\t%d=%d:%c:%x", i, u1_rcv_char[i],u1_rcv_char[i],u1_rcv_char[i]);
				}
				uart1_rcv_isr_count = 0;
				memset(u1_rcv_char,'\0', UARTX_BUFFER_SIZE);
				
			}
	
	};


	return 0;
}





