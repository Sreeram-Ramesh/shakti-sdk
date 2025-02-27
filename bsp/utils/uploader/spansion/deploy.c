/***************************************************************************
 * Project                               :  shakti devt board
 * Name of the file                      :  deploy.c
 * Brief Description of file             :  Deploy elf into flash in board.
 * Name of Author                        :  Sathya Narayanan N & Anand Kumar S
 * Email ID                              :  sathya281@gmail.com

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
@file deploy.c
@brief Deploy elf into flash in board.
@detail This file has functions that can write an array of hex numbers into the
flash device. SPI protocol is used for this purporse. The array is usually a
project's hex file 
*/

#include <stdint.h>
#include <uart.h>
#include <stdio.h>
#include "sspi.h"
#include "sspi_spansion.h"
#include "flashdata.h"


#define WRITE_SIZE 8193
#define READ_SIZE 8193
// #define WRITE_SIZE 5
// #define READ_SIZE 5

#define FLASH_BASE_ADDRESS 0x00b00000
#define SSPI_INSTANCE 0

/** @fn static void flash_read_locations(uint32_t read_address, uint16_t length)
 * @brief Reads the flash memory contents starting from read_address.
          Totally length number of bytes will be read.
 * @param unsigned int (32-bit).
 * @param unsigned int (16-bit).           
 */

static void flash_read_locations(uint32_t read_address, uint32_t length, int *read_data)
{
	for(int i = 0; i < length; ++i)
	{
		int read_value = flash_read(read_address);
//		printf("\nReading from adddress %x and data \
//			   %x\n",read_address,read_value);
		*read_data++ = read_value;
		read_address = read_address+4;
	}
}

/** @fn void main()
 * @brief Configures the SPI flash and writes into a flash location.
 * @details Configures the SPI flash, Confirms the flash device id,erases a sector
 *           and then write into a flash location and prints the value.
 */
void main()
{
	int write_address = FLASH_BASE_ADDRESS;  // read/write from/to this address
	int read_address  = FLASH_BASE_ADDRESS;  // read/write from/to this address
	int data = 0xDEADBEEF; //32 bits of data can be written at a time
	uint32_t length = 11;

	sspi_init();
	flash_init();
	sspi_clear_fifo(sspi_instance[SSPI_INSTANCE]);
	printf("\nSSPI init done\n");
	flash_register_read(0x35);
	flash_register_read(0x05);
	flash_write_bank_register(0x17, 0x80);
	flash_register_read(0x16);
	flash_device_id();
	waitfor(200);
	printf("\n Flash device id read complete");
	flash_erase(FLASH_BASE_ADDRESS); //erases an entire sector
	printf("\nErase complete");

	int read_data[READ_SIZE] = {0};
	printf("\n Data read after erase:")	;
	length = READ_SIZE;
//	for(int i = 0; i < length; i++)
//	{
		flash_read_locations( read_address, length, &read_data);
//	}
//	while(1);
	printf("\n Flash read complete");
	//length = WRITE_SIZE;
	data = 0x0;
	for(int i = 0; i < length; i++)
	{
		flash_write( write_address, write_data[i]);
		write_address += 0x04;
	}
	length = READ_SIZE;
	//flash_read_locations( read_address, length, &read_data);
	flash_read_locations( read_address, length, &read_data);

	for(int i = 0; i < length; i++)
	{
		printf("\n read_data[%d] : %x",i,read_data[i]);
		if(read_data[i] != write_data[i] )
		{
			printf("\n Upload to Flash failed");
			printf("\n write_value: %x; read_value: %x; at location %x", write_data[i], read_data[i],i);
			asm volatile ("ebreak");
		}
	}
	printf("\n Upload completed");

	asm volatile ("ebreak");
}
