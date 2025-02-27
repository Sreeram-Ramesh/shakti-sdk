/***************************************************************************
 * Project               	    	: "iisu project"
 * Name of the file	            	: sspi_driver.c
 * Brief Description of file            : Driver file to Standard SPI
 * Name of Author    	                : Sambhav Jain
 * Email ID                             : sambhav.jv@gmail.com;

  Copyright (C) 2021  IIT Madras. All rights reserved.

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
/*
  @file spi.c
  @brief read data from the flash by spi
  @detail Contains driver codes to read and write flash using SPI interface.
 */

#include "sspi.h"
#include "uart.h"
#include "utils.h"

//#define DISABLE_CS 1
//#define SPI_FULL_DUPLEX 1
#define SPI_HALF_DUPLEX 1
//#define SHOW_REG 1

#define SSPI_DRIVER     1
#define FAST_READ 1
#define FLASH_WRITE_COMMAND 0x12
#define FLASH_FAST_READ_COMMAND 0x0C
#define FLASH_ERASE_COMMAND 0xDC
#define FLASH_READ_DEVICE_ID 0x9F
#define FLASH_READ_SR 0x05
#define FLASH_WRITE_SR 0x06
#define FLASH_WRITE_ENABLE 0x06
#define FLASH_NORMAL_READ_COMMAND 0x13


#define SSPI_INSTANCE 0


//sspi_struct *SSPI0 = (sspi_struct *) SSPI0_BASE_ADDRESS;

void flash_init()
{
 	sspi_enable_txrx(sspi_instance[SSPI_INSTANCE], DISABLE);
	sspi_configure_clock_in_hz(sspi_instance[SSPI_INSTANCE], 1000000); /* Currently configuring for 100kHz, as the maximum clock is 20MHz */
	sspi_configure_clock_pol(sspi_instance[SSPI_INSTANCE], 1);
	sspi_configure_clock_phase(sspi_instance[SSPI_INSTANCE], 1);	
	sspi_configure_pins(sspi_instance[SSPI_INSTANCE], 1, 0 ,  1 , 1);
	sspi_configure_mas_slv(sspi_instance[SSPI_INSTANCE], SPI_MASTER);
	sspi_configure_lsb_first(sspi_instance[SSPI_INSTANCE], MSB_FIRST);
	sspi_configure_tx_setup_time(sspi_instance[SSPI_INSTANCE], 1);
//	sspi_configure_tx_hold_time(sspi_instance[SSPI_INSTANCE], 0);
}

int flash_write_enable(void)
{
#ifdef DISABLE_CS
	sspi_enable_txrx(sspi_instance[SSPI_INSTANCE], DISABLE);
#endif
//	sspi_configure_tx_setup_time(sspi_instance[SSPI_INSTANCE], 2);
//	sspi_configure_tx_hold_time(sspi_instance[SSPI_INSTANCE], 2);
 	sspi_configure_tx_rx_length(sspi_instance[SSPI_INSTANCE],8,0);//Tx - 8, Rx - 0;
	sspi_configure_comm_mode(sspi_instance[SSPI_INSTANCE], SIMPLEX_TX);
	sspi_instance[SSPI_INSTANCE]->data_tx = FLASH_WRITE_ENABLE;	//MSB byte of address.
 	sspi_enable_txrx(sspi_instance[SSPI_INSTANCE], ENABLE);
 	sspi_notbusy();
	 log_debug("\n Flash write enabled");
 	return 1;
}

int sspi_notbusy(void)
{
	unsigned char  read_data = 0x01;
	while(read_data & 0x01){
		waitfor(10);
		read_data = sspi_instance[SSPI_INSTANCE]->comm_status;
//		read_data = SSPI0->comm_status;
		log_debug("\n Status Register: %x", read_data);
	}
	log_debug("\n SPI comm done \n");
	return 1;
}


char flash_cmd_read(char command)
{
	char recvDATA;
#ifdef DISABLE_CS
	sspi_enable_txrx(sspi_instance[SSPI_INSTANCE], DISABLE);
#endif
//	sspi_configure_tx_hold_time(sspi_instance[SSPI_INSTANCE], 2);
	sspi_instance[SSPI_INSTANCE]->data_tx = command;
#ifdef SPI_FULL_DUPLEX
	sspi_instance[SSPI_INSTANCE]->data_tx = 0xFF;	//FULL Duplex dummy write
	sspi_configure_tx_rx_length(sspi_instance[SSPI_INSTANCE], 16, 16);
	sspi_configure_comm_mode(sspi_instance[SSPI_INSTANCE], FULL_DUPLEX);
#endif
#ifdef SPI_HALF_DUPLEX
	sspi_configure_tx_rx_length(sspi_instance[SSPI_INSTANCE], 8, 8);
	sspi_configure_comm_mode(sspi_instance[SSPI_INSTANCE], HALF_DUPLEX);
#endif
	sspi_enable_txrx(sspi_instance[SSPI_INSTANCE], ENABLE);
	sspi_notbusy();	
#ifdef SPI_FULL_DUPLEX
	recvDATA = sspi_instance[SSPI_INSTANCE]->data_rx;
#endif
	recvDATA = sspi_instance[SSPI_INSTANCE]->data_rx;
	return recvDATA;
}

int flash_status_register_read(void)
{
	char stat = 0x3;
	stat = flash_cmd_read(FLASH_READ_SR);
	//printf("flash status register val %x\n", stat);
	while (stat & 0x03)
	{
		stat = flash_cmd_read(FLASH_READ_SR);
		log_debug("flash status register val %x\n", stat);
		//printf("flash status register val %x\n", stat);
	}

	return 0;
}

char flash_register_read(char command)
{
	char stat = 0x0;

		stat = flash_cmd_read(command);
		printf("\ncommand [%x] read value: %x\n", command, stat);

	return stat;
}


/**
 * @fn int flash_cmd_addr(int command, int addr)
 * @brief Use for sending 8bit of command + 32 bit of address 
 * @details Useful for function like erase
 * @warning to move data drom dr register to fifo there must be some data into spi_dr5 
 * @param int (command (opcode))
 * @param int (addr (address after the opcode))
 * @return int
 */
char flash_write_bank_register(char command, char value)
{

	sspi_instance[SSPI_INSTANCE]->data_tx = command;	//Do not shift  command when passing as argument.
	sspi_instance[SSPI_INSTANCE]->data_tx = value;	//MSB byte of address.
	sspi_configure_tx_rx_length(sspi_instance[SSPI_INSTANCE], 16, 0);//Tx - 16, Rx - 0;
	sspi_configure_comm_mode(sspi_instance[SSPI_INSTANCE], SIMPLEX_TX);
	sspi_enable_txrx(sspi_instance[SSPI_INSTANCE], ENABLE);
	waitfor(20);
	sspi_notbusy();
	log_debug("\n Flash write register is sent");
	return 1;
}



/**
 * @fn int flash_cmd_addr(int command, int addr)
 * @brief Use for sending 8bit of command + 32 bit of address 
 * @details Useful for function like erase
 * @warning to move data drom dr register to fifo there must be some data into spi_dr5 
 * @param int (command (opcode))
 * @param int (addr (address after the opcode))
 * @return int
 */
char flash_write_register(char command, char value1, char value2)
{

	sspi_instance[SSPI_INSTANCE]->data_tx = command;	//Do not shift  command when passing as argument.
	sspi_instance[SSPI_INSTANCE]->data_tx = value1;	//MSB byte of address.
	sspi_instance[SSPI_INSTANCE]->data_tx = value2;	//2nd byte of address.
	sspi_configure_tx_rx_length(sspi_instance[SSPI_INSTANCE], 24, 0);//Tx - 40, Rx - 0;
	sspi_configure_comm_mode(sspi_instance[SSPI_INSTANCE], SIMPLEX_TX);
	sspi_enable_txrx(sspi_instance[SSPI_INSTANCE], ENABLE);
	waitfor(20);
	sspi_notbusy();
	log_debug("\n Flash write register is sent");
	return 1;
}


/**
 * @fn int flash_cmd_addr(int command, int addr)
 * @brief Use for sending 8bit of command + 32 bit of address 
 * @details Useful for function like erase
 * @warning to move data drom dr register to fifo there must be some data into spi_dr5 
 * @param int (command (opcode))
 * @param int (addr (address after the opcode))
 * @return int
 */
int flash_cmd_addr(int command, int addr)
{

	sspi_instance[SSPI_INSTANCE]->data_tx =command;	//Do not shift  command when passing as argument.
	sspi_instance[SSPI_INSTANCE]->data_tx =(addr >> 24);	//MSB byte of address.
	sspi_instance[SSPI_INSTANCE]->data_tx =(addr >> 16);	//2nd byte of address.
	sspi_instance[SSPI_INSTANCE]->data_tx =(addr >> 8);	//3rd byte of address.
	sspi_instance[SSPI_INSTANCE]->data_tx =(addr >> 0);	//4th byte of address.
	sspi_configure_tx_rx_length(sspi_instance[SSPI_INSTANCE], 40, 0);//Tx - 40, Rx - 0;
	sspi_configure_comm_mode(sspi_instance[SSPI_INSTANCE], SIMPLEX_TX);
	sspi_enable_txrx(sspi_instance[SSPI_INSTANCE], ENABLE);
	waitfor(20);
	sspi_notbusy();
	log_debug("\n Flash Erase command is sent");
	return 1;
}

void flash_erase(int address)
{
#ifdef DISABLE_CS
	sspi_enable_txrx(sspi_instance[SSPI_INSTANCE], DISABLE);
#endif
	flash_write_enable();
	flash_cmd_addr(FLASH_ERASE_COMMAND, address);
	flash_status_register_read();
	log_debug("\n Flash erase is done");
}


/**
 * @fn void flash_write(int address, int data)
 * @brief  Write 4bytes of data from given address
 * @details flash_cmd_addr_data with opcode 12h.  
 * @warning before writing into the flash one should enable the WEL bit spi_sr by using write_enable(void)
 * @param int (addres (write address))
 * @param int(data (write data))
 */
void flash_write(int address, int data)
{
	log_debug("\n Write data %x at address %x", data, address);
#ifdef DISABLE_CS
	sspi_enable_txrx(sspi_instance[SSPI_INSTANCE], DISABLE);
#endif
	flash_write_enable();
	sspi_instance[SSPI_INSTANCE]->data_tx = FLASH_WRITE_COMMAND;	//Do not shift  command when passing as argument.
	sspi_instance[SSPI_INSTANCE]->data_tx = (address >> 24);	//MSB byte of address.
	sspi_instance[SSPI_INSTANCE]->data_tx = (address >> 16);	//2nd byte of address.
	sspi_instance[SSPI_INSTANCE]->data_tx = (address >> 8);	//3rd byte of address.
	sspi_instance[SSPI_INSTANCE]->data_tx = (address >> 0);	//4th byte of address.
	sspi_instance[SSPI_INSTANCE]->data_tx = (data >> 24);	//4th byte of data.
	sspi_instance[SSPI_INSTANCE]->data_tx = (data >> 16);	//4th byte of data.
	sspi_instance[SSPI_INSTANCE]->data_tx = (data >> 8);	//4th byte of data.
	sspi_instance[SSPI_INSTANCE]->data_tx = (data >> 0);	//4th byte of data.
	sspi_configure_tx_rx_length(sspi_instance[SSPI_INSTANCE], 72, 0);//Tx - 72, Rx - 0;
	sspi_configure_comm_mode(sspi_instance[SSPI_INSTANCE], SIMPLEX_TX);
	sspi_enable_txrx(sspi_instance[SSPI_INSTANCE], ENABLE);
#ifdef SHOW_REG
	sspi_read_registers(sspi_instance[SSPI_INSTANCE]);
#endif	
	sspi_notbusy();
	flash_status_register_read();
	log_debug("\n Flash data is written");
}

/** @fn int flash_read(int address)
 * @brief read the 4bytes data from given address 
 * @details flash_cmd_to_read with opcode 0Bh for fast read
 * @param int (address (read address))
 * @return int 
 */
int flash_read(int address)
{
	int recv_data = 0;
	char read_data[4] = {'\0'};
#ifdef DISABLE_CS
	sspi_enable_txrx(sspi_instance[SSPI_INSTANCE], DISABLE);
#endif
#ifdef FAST_READ
#ifdef SPI_HALF_DUPLEX
	sspi_instance[SSPI_INSTANCE]->data_tx =FLASH_FAST_READ_COMMAND;	//Do not shift  command when passing as argument.
	sspi_instance[SSPI_INSTANCE]->data_tx =(address >> 24);	//MSB byte of address.
	sspi_instance[SSPI_INSTANCE]->data_tx =(address >> 16);	//2nd byte of address.
	sspi_instance[SSPI_INSTANCE]->data_tx =(address >> 8);	//3rd byte of address.
	sspi_instance[SSPI_INSTANCE]->data_tx =(address >> 0);	//4th byte of address.
	sspi_instance[SSPI_INSTANCE]->data_tx = 0xFF;	//Dummy byte.
	sspi_configure_tx_rx_length(sspi_instance[SSPI_INSTANCE], 48, 32);//Tx - 48, Rx - 32;
	sspi_configure_comm_mode(sspi_instance[SSPI_INSTANCE], HALF_DUPLEX);
	sspi_enable_txrx(sspi_instance[SSPI_INSTANCE], ENABLE);
#endif
#ifdef SPI_FULL_DUPLEX
	sspi_instance[SSPI_INSTANCE]->data_tx =FLASH_FAST_READ_COMMAND;	//Do not shift  command when passing as argument.
	sspi_instance[SSPI_INSTANCE]->data_tx =(address >> 24);	//MSB byte of address.
	sspi_instance[SSPI_INSTANCE]->data_tx =(address >> 16);	//2nd byte of address.
	sspi_instance[SSPI_INSTANCE]->data_tx =(address >> 8);	//3rd byte of address.
	sspi_instance[SSPI_INSTANCE]->data_tx =(address >> 0);	//4th byte of address.
	sspi_instance[SSPI_INSTANCE]->data_tx = 0xFF;	//Dummy byte.
	sspi_instance[SSPI_INSTANCE]->data_tx = 0xFF;	//Full Duplex Dummy byte.
	sspi_instance[SSPI_INSTANCE]->data_tx = 0xFF;	//Full Duplex Dummy byte.
	sspi_instance[SSPI_INSTANCE]->data_tx = 0xFF;	//Full Duplex Dummy byte.
	sspi_instance[SSPI_INSTANCE]->data_tx = 0xFF;	//Full Duplex Dummy byte.
	sspi_configure_tx_rx_length(sspi_instance[SSPI_INSTANCE], 80, 80);//Tx - 48, Rx - 32;
	sspi_configure_comm_mode(sspi_instance[SSPI_INSTANCE], FULL_DUPLEX);
	sspi_enable_txrx(sspi_instance[SSPI_INSTANCE], ENABLE);
#endif
#endif
#ifdef NORMAL_READ
	sspi_instance[SSPI_INSTANCE]->data_tx =FLASH_NORMAL_READ_COMMAND;	//Do not shift  command when passing as argument.
	sspi_instance[SSPI_INSTANCE]->data_tx =(address >> 24);	//MSB byte of address.
	sspi_instance[SSPI_INSTANCE]->data_tx =(address >> 16);	//2nd byte of address.
	sspi_instance[SSPI_INSTANCE]->data_tx =(address >> 8);	//3rd byte of address.
	sspi_instance[SSPI_INSTANCE]->data_tx =(address >> 0);	//4th byte of address.
	sspi_configure_tx_rx_length(sspi_instance[SSPI_INSTANCE], 72, 72);//Tx - 48, Rx - 32;
//	sspi_configure_comm_mode(sspi_instance[SSPI_INSTANCE], HALF_DUPLEX);
	sspi_configure_comm_mode(sspi_instance[SSPI_INSTANCE], FULL_DUPLEX);
	sspi_enable_txrx(sspi_instance[SSPI_INSTANCE], ENABLE);
#endif

#ifdef SHOW_REG
	sspi_read_registers(sspi_instance[SSPI_INSTANCE]);
#endif	
	sspi_notbusy();
#ifdef SPI_FULL_DUPLEX
	read_data[0] = sspi_instance[SSPI_INSTANCE]->data_rx; //Full Duplex Dummy read
	read_data[0] = sspi_instance[SSPI_INSTANCE]->data_rx; //Full Duplex Dummy read
	read_data[0] = sspi_instance[SSPI_INSTANCE]->data_rx; //Full Duplex Dummy read
	read_data[0] = sspi_instance[SSPI_INSTANCE]->data_rx; //Full Duplex Dummy read
	read_data[0] = sspi_instance[SSPI_INSTANCE]->data_rx; //Full Duplex Dummy read
	read_data[0] = sspi_instance[SSPI_INSTANCE]->data_rx; //Full Duplex Dummy read
#endif	
	read_data[0] = sspi_instance[SSPI_INSTANCE]->data_rx;
	read_data[1] = sspi_instance[SSPI_INSTANCE]->data_rx;
	read_data[2] = sspi_instance[SSPI_INSTANCE]->data_rx;
	read_data[3] = sspi_instance[SSPI_INSTANCE]->data_rx;
	recv_data = ( (read_data[0] << 24) | (read_data[1] << 16) | (read_data[2] << 8) | (read_data[3] << 0) );
//	printf("\n d[0]: %x; d[1]: %x;  d[2]: %x;  d[4]: %x; recv_data: %x", read_data[0], read_data[1], read_data[2], read_data[3], recv_data) ;
	return recv_data;
}


int flash_device_id(void)
{

	uint32_t recvData = 0;
	uint8_t read_data[6] = {'\0'};
	flash_write_enable();
#ifdef DISABLE_CS
	sspi_enable_txrx(sspi_instance[SSPI_INSTANCE], DISABLE);
#endif
	sspi_instance[SSPI_INSTANCE]->data_tx = FLASH_READ_DEVICE_ID;		
#ifdef SPI_HALF_DUPLEX
	sspi_configure_tx_rx_length(sspi_instance[SSPI_INSTANCE], 8, 48);//Tx - 8, Rx - 24;
	sspi_configure_comm_mode(sspi_instance[SSPI_INSTANCE], HALF_DUPLEX);
#endif
#ifdef SPI_FULL_DUPLEX
	sspi_instance[SSPI_INSTANCE]->data_tx = 0xff; //Full duplex dummy write
	sspi_instance[SSPI_INSTANCE]->data_tx = 0xff; //Full duplex dummy write
	sspi_instance[SSPI_INSTANCE]->data_tx = 0xff; //Full duplex dummy write
	sspi_configure_tx_rx_length(sspi_instance[SSPI_INSTANCE], 32, 32);//Tx - 8, Rx - 24;
	sspi_configure_comm_mode(sspi_instance[SSPI_INSTANCE], FULL_DUPLEX);
#endif
#ifdef SHOW_REG
	sspi_read_registers(sspi_instance[SSPI_INSTANCE]);
#endif	
	sspi_enable_txrx(sspi_instance[SSPI_INSTANCE], ENABLE);
	sspi_notbusy();

#ifdef SPI_FULL_DUPLEX
	//Full duplex dummy read
	recvData = sspi_instance[SSPI_INSTANCE]->data_rx; //Full Duplex Dummy read
#endif
//	recvData = (sspi_instance[SSPI_INSTANCE]->data_rx << 16) | \
	(sspi_instance[SSPI_INSTANCE]->data_rx << 8 ) | (sspi_instance[SSPI_INSTANCE]->data_rx );
//	printf("\n Device id is %x", recvData);
/*	read_data[0] = sspi_instance[SSPI_INSTANCE]->data_rx;
	read_data[1] = sspi_instance[SSPI_INSTANCE]->data_rx;
	read_data[2] = sspi_instance[SSPI_INSTANCE]->data_rx;
	read_data[3] = sspi_instance[SSPI_INSTANCE]->data_rx;
	read_data[4] = sspi_instance[SSPI_INSTANCE]->data_rx;
	read_data[5] = sspi_instance[SSPI_INSTANCE]->data_rx;
*/

	for (int i = 0; i < 6; i++)
	{
		read_data[i] = sspi_instance[SSPI_INSTANCE]->data_rx;
		printf("\n data[%x]: %x", i, read_data[i]);
	}

	recvData = read_data[0] << 16 | read_data[1] << 8 | read_data[2];
	printf("\n Device id is %x", recvData);
	return recvData;	
}
