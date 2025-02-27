/***************************************************************************
 * Project               	    	: "iisu project"
 * Name of the file	            	: sspi_spansion.h
 * Brief Description of file            : Driver file to Standard SPI Spansion flash
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

#ifndef SSPI_SPANSION_H
#define SSPI_SPANSION_H
void flash_init();
int flash_write_enable(void);
int sspi_notbusy(void);
char flash_cmd_read(char command);
int flash_status_register_read(void);
char flash_register_read(char command);
char flash_write_register(char command, char value1, char value2);
int flash_cmd_addr(int command, int addr);
void flash_erase(int address);
void flash_write(int address, int data);
int flash_read(int address);
int flash_device_id(void);
#endif