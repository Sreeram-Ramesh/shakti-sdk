/***************************************************************************
 * Project                           : "iisu project"
 * Name of the file                  : sspi.h
 * Brief Description of file         : Header to Standard spi driver
 * Name of Author                    : Sambhav Jain
 * Email ID                          : sambhav.jv@gmail.com

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
 * @file sspi.h
 * @project iisu project
 * @brief Header to sspi driver 
 */
#ifndef _SSPI_H
#define _SSPI_H
// #include <stdlib.h>
#include <stdint.h>
#include "platform.h"
#include "log.h"


typedef struct 
{
    uint32_t comm_control;        /*! 32 bits -SSPI Communication Control Register*/
    uint32_t clk_control;         /*! 32 bits -SSPI Clock Control Register */
    uint8_t data_tx;             /*! 32 bits -SSPI Transmitter Data Register */
    uint8_t reserve_data_tx1;            /*! 16 bits -Reserved */  
    uint16_t reserve_data_tx;            /*! 16 bits -Reserved */
    uint8_t data_rx;             /*! 32 bits -SSPI Receiver Data Register */
    uint8_t reserve_data_rx1;            /*! 16 bits -Reserved */ 
    uint16_t reserve_data_rx;            /*! 16 bits -Reserved */
    uint16_t intr_en;             /*! 16 bits -SSPI Interrupt Enable Register */
    uint16_t reserve0;            /*! 16 bits -Reserved */
    uint8_t fifo_status;          /*!  8 bits -SSPI FIFO Status Register */ 
    uint8_t reserve1;             /*!  8 bits -Reserved */
    uint16_t reserve2;            /*! 16 bits -Reserved */
    uint8_t comm_status;          /*!  8 bits -SSPI Communication Status Register */
    uint8_t reserve3;             /*!  8 bits -Reserved */
    uint16_t reserve4;            /*! 16 bits -Reserved */
    uint8_t qual;                 /*!  8 bits -SSPI Input Qualification Control Register */
    uint8_t reserve5;             /*!  8 bits -Reserved */
    uint16_t reserve6;            /*! 16 bits -Reserved */
}sspi_struct;


/* Declaring the buffer variables to be used in ISR */
#ifdef SSPI_DRIVER
/* Mapping SSPI0 Base address to the struct instance of SSPI0 */
sspi_struct *sspi_instance[SSPI_MAX_COUNT];
uint32_t buf_length =0;
uint32_t buf_data =0;; 
#else
// /* Mapping SSPI0 Base address to the struct instance of SSPI0 */
extern sspi_struct *sspi_instance[];
extern uint32_t buf_length;
extern uint32_t buf_data; 
#endif

/*! SSPIx Communication Control Register */
#define SPI_MASTER             (1<<0)  
#define SPI_ENABLE             (1<<1)
#define SPI_LSB_FIRST          (1<<2)
#define SPI_COMM_MODE(x)       (x<<4)
#define SPI_TOTAL_BITS_TX(x)   (x<<6)
#define SPI_TOTAL_BITS_RX(x)   (x<<14)
#define SPI_OUT_EN_SCLK        (1<<22)
#define SPI_OUT_EN_NCS         (1<<23)
#define SPI_OUT_EN_MISO        (1<<24)
#define SPI_OUT_EN_MOSI        (1<<25)

/*! SSPIx Clock Control Register */
#define SPI_CLK_POLARITY      (1<<0)
#define SPI_CLK_PHASE         (1<<1)
#define SPI_PRESCALE(x)       (x<<2)
#define SPI_SS2TX_DELAY(x)    (x<<10)
#define SPI_TX2SS_DELAY(x)    (x<<18)

/*! SSPIx Interrupt Enable Register */
#define SPI_TX_EMPTY_INTR_EN    (1<<0)
#define SPI_TX_QUAD_INTR_EN     (1<<1)
#define SPI_TX_HALF_INTR_EN     (1<<2)
#define SPI_TX_FULL_INTR_EN     (1<<3)
#define SPI_RX_EMPTY_INTR_EN    (1<<4)
#define SPI_RX_QUAD_INTR_EN     (1<<5)
#define SPI_RX_HALF_INTR_EN     (1<<6)
#define SPI_RX_FULL_INTR_EN     (1<<7)
#define SPI_RX_OVERRUN_INTR_EN  (1<<8)

/*! SSPIx Communication Status Register */
#define SPI_BUSY             (1<<0)
#define SPI_TXE              (1<<1)
#define SPI_RXNE             (1<<2)
#define SPI_TX_FIFO          (3<<3)
#define SPI_RX_FIFO          (3<<5)
#define SPI_OVR              (1<<7)

/*! SSPIx FIFO Status Register */
#define SPI_TX_EMPTY          (1<<0)
#define SPI_TX_QUAD           (1<<1)
#define SPI_TX_HALF           (1<<2)
#define SPI_TX_FULL           (1<<3)
#define SPI_RX_EMPTY          (1<<4)
#define SPI_RX_QUAD           (1<<5)
#define SPI_RX_HALF           (1<<6)
#define SPI_RX_FULL           (1<<7)

#define MASTER 1
#define SLAVE 0

#define DISABLE 0
#define ENABLE 1

#define LSB_FIRST 1
#define MSB_FIRST 0

#define SIMPLEX_TX 0
#define SIMPLEX_RX 1
#define HALF_DUPLEX 2
#define FULL_DUPLEX 3

#define SUCCESS 0
#define FAILURE -1
#define TIMEOUT -2


/** @fn void sspi_configure_mas_slv(sspi_struct *sspi_instance, uint8_t mas_slv_cntrl) 
 * @brief configuring the master or slave configuration 
 * @details  Setting the SPIMASTER bit on communication control register 
 * @param sspi instance pointer holds the instance of sspi_struct
 * @param mas_slv_cntrl decides if the configuration is master or slave
 */
void sspi_configure_mas_slv(sspi_struct *sspi_instance, uint8_t mas_slv_cntrl);

/** @fn void sspi_configure_pins(sspi_struct *sspi_instance, uint8_t mosi_pin_cntrl,uint8_t miso_pin_cntrl,uint8_t cs_pin_cntrl,uint8_t clk_pin_cntrl)
 * @brief configuring the SSPI pins as input or output  
 * @details Configuring all the four pins (MISO,MOSI,CS and CLK) as outputs or inputs in Comm Control Register  
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param mosi_pin_cntrl Output enable for MOSI pin. When set(1), MOSI will be an output and when reset(0), it will be an input.
 * @param miso_pin_cntrl Output enable for MISO pin. When set(1), MISO will be an output and when reset(0), it will be an input.
 * @param cs_pin_cntrl   Output enable for CS pin. When set(1),   CS will be an output and when reset(0), it will be an input.
 * @param clk_pin_cntrl  Output enable for CLK pin. When set(1),  CLK will be an output and when reset(0), it will be an input.
 */
void sspi_configure_pins(sspi_struct *sspi_instance, uint8_t mosi_pin_cntrl,uint8_t miso_pin_cntrl,uint8_t cs_pin_cntrl,uint8_t clk_pin_cntrl);

/** @fn sspi_configure_tx_rx_length(sspi_struct *sspi_instance, uint8_t tx_length, uint8_t rx_length)
 * @brief configuring the total bits to be transmitted and to be received  
 * @details Configuring the length of tx and rx bits and settting it in comm control register
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param tx_length Total number of bits to be sent.The values vary from 0 to 255 
 * @param rx_length Total number of bits to be received.The values vary from 0 to 255
*/
void sspi_configure_tx_rx_length(sspi_struct *sspi_instance, uint8_t tx_length, uint8_t rx_length);

/** @fn sspi_configure_comm_mode(sspi_struct *sspi_instance, uint8_t comm_mode)
 * @brief configuring the communication mode in SSPI communication control register  
 * @details  Configuring the communication mode by setting SPICOMMMODE bits[5:4] 
 *          1)00-Simplex Transmit  2)01-Simplex Receive 3)10-Half Duplex  4)11-Full Deplex
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param comm_mode Setting communication mode value ranges from 0-3 
*/
void sspi_configure_comm_mode(sspi_struct *sspi_instance, uint8_t comm_mode);

/** @fn void sspi_configure_lsb_first(sspi_struct *sspi_instance, uint8_t lsb_enable)
 * @brief Enables the sspi to send / receive LSB first.   
 * @details If lsb_enable is set to 1, the sspi peripheral is enabled to send/receive LSB first.
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param lsb_enable If set 1, enables sspi to set SPILSBFIRST bit in comm control register to 1
*/
void sspi_configure_lsb_first(sspi_struct *sspi_instance, uint8_t lsb_enable);

/** @fn void sspi_enable_txrx(sspi_struct *sspi_instance, uint8_t enable)
 * @brief Enable the sspi to start tx or rx.
 * @details SPIENABLE bit in comm_control register is set to 1, SSPI peripheral starts to transmit or receive as per configuration
 * @warning
 * @param sspi_instance sspi_instance pointer holds the instance of sspi_struct
 * @param enable If 1, enables the sspi for transmit or receive
 */
void sspi_enable_txrx(sspi_struct *sspi_instance, uint8_t enable);

/** @fn void sspi_configure_tx_setup_time(sspi_struct *sspi_instance, uint8_t delay_value)
 * @brief Configures the sspi setup time.
 * @details Setup time is the Chip select high to transmission delay and holds the number of sspi clock cycle delay before transmission.
 * The required delay will be programmed into the clk register.
 * It takes values from 0 to 255 indicating 0 to 255 SPI clock cycles
 * @warning
 * @param sspi_instance sspi_instance pointer holds the instance of sspi_struct
 * @param delay_value value of delay ranges from 0 to 255
 */
void sspi_configure_tx_setup_time(sspi_struct *sspi_instance, uint8_t delay_value);

/** @fn void sspi_configure_tx_hold_time(sspi_struct *sspi_instance, uint8_t delay_value) 
 * @brief Configures the sspi hold time
 * @details Hold time is Transmission end to Chip select low and holds the number of sspi clock cycle delay after transmission
 * The required delay will be programmed into the clk register by values ranging from 0 to 255
 * @param sspi_instance 
 * @param delay_value 
 */
void sspi_configure_tx_hold_time(sspi_struct *sspi_instance, uint8_t delay_value);

/** @fn void sspi_configure_clock_in_hz(sspi_struct *sspi_instance, uint32_t bit_rate)
 * @brief Configures the prescaler value for the passed bit_rate.
 * @details Calculates the prescaler value from the passed bit_rate and updates the prescaler value in clock control register.
 * @warning
 * @param sspi_instance 
 * @param bit_rate input is given to calculate the prescaler value 
 */
void sspi_configure_clock_in_hz(sspi_struct *sspi_instance, uint32_t bit_rate);

/** @fn void sspi_configure_clock_prescaler(sspi_struct *sspi_instance, uint8_t prescaler)
 * @brief Configures the prescaler value for the clock
 * @details Updates the prescaler value in clock control register
 * @warning
 * @param sspi_instance 
 * @param prescaler Prescaler value takes from 0 to 255 
 */
void sspi_configure_clock_prescaler(sspi_struct *sspi_instance, uint8_t prescaler);

/** @fn void sspi_configure_clock_phase(sspi_struct *sspi_instance, uint8_t clk_phase)
 * @brief Configures the phase value for the clock register
 * @details If phase = 1, sspi transmits or receives data in second clock transition else first clock transition 
 * @warning
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param clk_phase clk_phase takes value 0 or 1
 */
void sspi_configure_clock_phase(sspi_struct *sspi_instance, uint8_t clk_phase);

/** @fn void sspi_configure_clock_pol(sspi_struct *sspi_instance, uint8_t clk_pol)
 * @brief Configures the polarity value for the clock register
 * @details If polarity = 1, default value of the clock is high, else default value of clock line is low
 * @warning
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param clk_pol clk_pol takes value 0 or 1
 */
void sspi_configure_clock_pol(sspi_struct *sspi_instance, uint8_t clk_pol);

/** @fn void sspi_configure_clock(sspi_struct *sspi_instance, uint8_t prescaler, uint8_t clk_pol, uint8_t clk_phase)
 * @brief Configures the prescaler, polarity and phase for clock used in sspi communication 
 * @details If polarity = 1, default value of the clock is high, else default value of clock line is low. 
 *          If phase = 1, sspi transmits or receives data in second clock transition else first clock transition
 * @param sspi_instance 
 * @param prescaler Prescaler value takes from 0 to 255 
 * @param clk_pol clk_polarity takes value 0 or 1
 * @param clk_phase clk_phase takes value 0 or 1
 */
void sspi_configure_clock(sspi_struct *sspi_instance, uint8_t prescaler, uint8_t clk_pol, uint8_t clk_phase);

/** @fn void sspi_read_comm_status(sspi_struct *sspi_instance, uint8_t *sts)
 * @brief Read the communication status register  
 * @details  
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param sts pointer which will hold communication status register value 
 */
void sspi_read_comm_status(sspi_struct *sspi_instance, uint8_t *sts);

/** @fn void sspi_read_fifo_status(sspi_struct *sspi_instance, uint8_t *fifo_sts)
 * @brief Reads the FIFO status register  
 * @details  
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param sts pointer which will hold FIFO status register value 
 */
void sspi_read_fifo_status(sspi_struct *sspi_instance, uint8_t *fifo_sts);

/** @fn void sspi_check_rx_fifo_full(sspi_struct *sspi_instance, uint8_t *sts)
 * @brief Checking if the Receiver FIFO level is full or not 
 * @details  It checks SPIRXFIFO bits in the Communication status register 
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param sts pointer which will hold 1- if the RX FIFO level is full or else 0. 
 */
void sspi_check_rx_fifo_full(sspi_struct *sspi_instance, uint8_t *sts);

/** @fn void sspi_check_tx_fifo_empty(sspi_struct *sspi_instance, uint8_t *sts)
 * @brief Checking if the tranmitter FIFO level is empty or not 
 * @details  It checks SPITXFIFO bits in the Communication status register 
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param sts pointer which will hold 1- if the TX FIFO level is empty or else 0. 
 */
void sspi_check_tx_fifo_empty(sspi_struct *sspi_instance, uint8_t *sts);


/** @fn void sspi_check_tx_fifo_level(sspi_struct *sspi_instance, uint8_t *sts)
 * @brief Checking the tranmitter FIFO level
 * @details  Reading the TX FIFO Level
 *          00-FIFO empty
 *          01-1/4 FIFO
 *          10-1/2 FIFO
 *          11-FIFO full
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param sts pointer which will hold TX FIFO level. 
 */
void sspi_check_tx_fifo_level(sspi_struct *sspi_instance, uint8_t *sts);

/** @fn void sspi_check_tx_fifo_level(sspi_struct *sspi_instance, uint8_t *sts)
 * @brief Checking the tranmitter FIFO level
 * @details  Reading the TX FIFO Level
 *          00-FIFO empty
 *          01-1/4 FIFO
 *          10-1/2 FIFO
 *          11-FIFO full
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param sts pointer which will hold TX FIFO level. 
 */
void sspi_check_tx_fifo_level(sspi_struct *sspi_instance, uint8_t *sts);

/** @fn void sspi_check_rx_fifo_level(sspi_struct *sspi_instance, uint8_t *sts)
 * @brief Checking the receiver FIFO level 
 * @details Reading the RX FIFO level  
 *          00-FIFO empty
 *          01-1/4 FIFO
 *          10-1/2 FIFO
 *          11-FIFO full
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param sts pointer which will hold RX FIFO level. 
 */
void sspi_check_rx_fifo_level(sspi_struct *sspi_instance, uint8_t *sts);

/** @fn void sspi_check_overrun_fifo(sspi_struct *sspi_instance, uint8_t *sts)
 * @brief Checking the overrun error in FIFO  
 * @details Reading SPI_OVR bit in status register  
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param sts pointer which will hold overrun error bit. 
 */
void sspi_check_rx_fifo_level(sspi_struct *sspi_instance, uint8_t *sts);

/** @fn void sspi_enable_tx_rx_interrupts(sspi_struct *sspi_instance, uint8_t tx_int_cntrl, uint8_t rx_int_cntrl)
 * @brief Enabling the respective interrupts for TX and RX FIFO  
 * @details Enabling the interrupt enable bits to get interrupt when TX or RX FIFO occurs.
 *          Bit[3:0] The bits which when set would trigger interrupts when TX FIFO is 
 *                  Bit 0 - empty; 1 - FIFO gets 8 entries; 2 - FIFO gets 16 entries; 3 - FIFO gets 32 entries.
 *          Bit[7:4] The bits which when set would trigger interrupts when RX FIFO is 
 *                  Bit 4 - empty; 5 - FIFO gets 8 entries; 6 - FIFO gets 16 entries; 7 - FIFO gets 32 entries.
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param tx_int_cntrl  Holds value from 0 to 15 to set interrupts on TX FIFO 
 * @param rx_int_cntrl  Holds value from 0 to 15 to set interrupts on RX FIFO
 */
void sspi_enable_tx_rx_interrupts(sspi_struct *sspi_instance, uint8_t tx_int_cntrl, uint8_t rx_int_cntrl);

/** @fn void sspi_enable_over_run_interrupt(sspi_struct *sspi_instance, uint8_t over_run_int_cntrl)
 * @brief Enabling the overrun interrupt 
 * @details Enabling the over run interrupt enable bits to get interrupt overrun.
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param over_run_int_cntrl  Holds value from 0 to 1 to set interrupts on SPI_RX_OVERRUN_INTR_EN bit.
 */
void sspi_enable_over_run_interrupt(sspi_struct *sspi_instance, uint8_t over_run_int_cntrl);

/** @fn void sspi_transmit_data(sspi_struct *sspi_instance, uint32_t *buf_data, uint8_t buf_length)
 * @brief To transmit data on TX data register
 * @details Transmitting data by writing buf_data on transmit register after checking the TXE bit on Status register.
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param buf_data this pointer holds the buffer data which has to be transmitted 
 * @param buf_length Holds the length of the buffer data sent
 */
void sspi_transmit_data(sspi_struct *sspi_instance, uint32_t *buf_data, uint8_t buf_length);


/** @fn void sspi_receive_data(sspi_struct *sspi_instance, uint32_t *buf_data)
 * @brief Receiving data on RX Data Register
 * @details Receivgin data by reading the RX register and writing the read data on buf_data.
 * @param[in] sspi_instance pointer holds the instance of sspi_struct
 * @param[in] buf_data this pointer holds the received data from the Data RX register 
 * @param[out] Returns success / failure status
 */
int sspi_receive_data(sspi_struct *sspi_instance, uint16_t *buf_data);


/** @fn unsigned int sspi_receive_n_data(sspi_struct *sspi_instance, uint32_t *buf_data, uint8_t buf_length)
 * @brief Receiving 'n' number of data on RX Data Register
 * @details Receivgin data by reading the RX register and writing the read data on buf_data.
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param buf_data this pointer holds the received data from the Data RX register 
 * @param buf_length points holding number of buffers received
 */
unsigned int sspi_receive_n_data(sspi_struct *sspi_instance, uint32_t *buf_data, uint8_t buf_length);

/** @fn void sspi_transmit_data(sspi_struct *sspi_instance, uint32_t*buf_data, uint8_t buf_length)
 * @brief To transmit data on TX data register
 * @details Transmitting data by writing buf_data on transmit register after checking the TXE bit on Status register.
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param buf_data this pointer holds the buffer data which has to be transmitted 
 * @param buf_length Holds the length of the buffer data sent
 */
void sspi_clear_fifo(sspi_struct *sspi_instance);

/** @fn void sspi_isr()
 * @brief Interrupt service routine for SSPI peripheral
 * @details This functions will be called to ofer interrupt service routine for SSPI0
 */
void sspi_isr_handler();

void sspi_read_registers(sspi_struct *sspi_instance);


#endif
