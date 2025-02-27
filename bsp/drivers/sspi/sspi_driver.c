/***************************************************************************
 * Project               	    		: "iisu project"
 * Name of the file	            		: sspi_driver.c
 * Brief Description of file            : Driver file to Standard SPI
 * Name of Author    	                : Sambhav Jain
 * Email ID                             : sambhav.jv@gmail.com;

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
 * @file sspi_driver.c
 * @project iisu project
 * @brief Driver file to sspi driver 
 */

#define SSPI_DRIVER  1
#include "sspi.h"
#include "platform.h"
#include "log.h"


/**
 * @fn sspi_init
 * @brief This function initialises the SSPI registers as array of pointers
 * @details  This function initialises the 
 * @param[in] No input parameters.
 * @param[Out] No ouput parameter 
 * @return Void function (Null)
 */
void sspi_init()
{
    for(int i = 0; i < SSPI_MAX_COUNT; i++)
    {
		sspi_instance[i] = (sspi_struct*) ( (SSPI0_BASE_ADDRESS + ( i * SSPI_BASE_OFFSET) ) );
        printf("\nsspi_instance[%x]: %x", i, sspi_instance[i]);
    }
}


/** @fn void sspi_configure_mas_slv(sspi_struct *sspi_instance, uint8_t mas_slv_cntrl) 
 * @brief configuring the master or slave configuration 
 * @details  Setting the SPI_MASTER bit on communication control register
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param mas_slv_cntrl decides if the configuration is master or slave
 */
void sspi_configure_mas_slv(sspi_struct *sspi_instance, uint8_t mas_slv_cntrl) 
{
    uint32_t temp = sspi_instance -> comm_control;
    mas_slv_cntrl &= 0x1;
    if(mas_slv_cntrl)
    {
        sspi_instance -> comm_control = temp | SPI_MASTER ;  
    }
    else
    {
        sspi_instance -> comm_control = temp & (~SPI_MASTER) ;
    }
}


/** @fn void sspi_configure_pins(sspi_struct *sspi_instance, uint8_t mosi_pin_cntrl,uint8_t miso_pin_cntrl,
 *      uint8_t  cs_pin_cntrl,uint8_t  clk_pin_cntrl)
 * @brief configuring the SSPI pins as input or output  
 * @details Configuring all the four pins (MISO,MOSI,CS and CLK) as outputs or inputs in Comm Control Register 
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param mosi_pin_cntrl Output enable for MOSI pin. When set(1), MOSI will be an output and when reset(0), it  
 *        will be an input.
 * @param miso_pin_cntrl Output enable for MISO pin. When set(1), MISO will be an output and when reset(0), it 
 *        will be an input.
 * @param cs_pin_cntrl   Output enable for CS pin. When set(1),   CS will be an output and when reset(0), it 
 *        will be an input.
 * @param clk_pin_cntrl  Output enable for CLK pin. When set(1),  CLK will be an output and when reset(0), it 
 *        will be an input.
 */
void sspi_configure_pins(sspi_struct *sspi_instance, uint8_t  mosi_pin_cntrl,uint8_t miso_pin_cntrl,uint8_t cs_pin_cntrl,uint8_t clk_pin_cntrl)
{
    uint32_t temp = sspi_instance -> comm_control;
    
    if(mosi_pin_cntrl){
        temp |= SPI_OUT_EN_MOSI;  
    }
    else{
        temp &= ~SPI_OUT_EN_MOSI;  
    }

    if(miso_pin_cntrl){
        temp |= SPI_OUT_EN_MISO;  
    }
    else{
        temp &= ~SPI_OUT_EN_MISO;  
    }

    if(cs_pin_cntrl){
        temp |= SPI_OUT_EN_NCS;  
    }
    else{
        temp &= ~SPI_OUT_EN_NCS;  
    }

    if(clk_pin_cntrl){
        temp |= SPI_OUT_EN_SCLK;  
    }
    else{
        temp &= ~SPI_OUT_EN_SCLK;  
    }

    sspi_instance -> comm_control=temp;
}


/** @fn void sspi_configure_tx_rx_length(sspi_struct *sspi_instance, uint8_t tx_length, uint8_t rx_length)
 * @brief configuring the total bits to be transmitted and to be recieved  
 * @details Configuring the length of tx and rx bits and settting it in comm control register
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param tx_length Total number of bits to be sent.The values vary from 0 to 255 
 * @param rx_length Total number of bits to be received.The values vary from 0 to 255
 */
void sspi_configure_tx_rx_length(sspi_struct *sspi_instance, uint8_t tx_length, uint8_t rx_length)
{       
    uint32_t temp = (sspi_instance -> comm_control) & ~(SPI_TOTAL_BITS_TX(255) | SPI_TOTAL_BITS_RX(255) );

    sspi_instance -> comm_control = (temp | SPI_TOTAL_BITS_TX(tx_length) | SPI_TOTAL_BITS_RX(rx_length) );
}


/** @fn void sspi_configure_comm_mode(sspi_struct *sspi_instance, uint8_t comm_mode)
 * @brief configuring the communication mode in SSPI communication control register  
 * @details Configuring the communication mode by setting SPI_COMM_MODE bits[5:4] 
 *          1)00-Simplex Transmit  2)01-Simplex Receive 3)10-Half Duplex  4)11-Full Deplex
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param comm_mode Setting communication mode value ranges from 0-3 
*/
void sspi_configure_comm_mode(sspi_struct *sspi_instance, uint8_t comm_mode)
{
    //Clearing the SPI_COMM_MODE bits[5:4] before setting the respective bits in temp
    uint32_t temp = ( sspi_instance -> comm_control ) & ~(SPI_COMM_MODE(3));
    sspi_instance -> comm_control = (temp | (SPI_COMM_MODE(comm_mode) ) );
}

/** @fn void sspi_configure_lsb_first(sspi_struct *sspi_instance, uint8_t lsb_enable)
 * @brief Enables the sspi to send / receive LSB first.   
 * @details If lsb_enable is set to 1, the sspi peripheral is enabled to send/receive LSB first.
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param lsb_enable If set 1, enables sspi to set SPI_LSB_FIRST bit in comm control register to 1
*/
void sspi_configure_lsb_first(sspi_struct *sspi_instance, uint8_t lsb_enable)
{
    //Clearing the SPI_COMM_MODE bits[5:4] before setting the respective bits in temp
    uint32_t temp = sspi_instance -> comm_control;
    lsb_enable &= 0x01;
    if(lsb_enable == 1)
    {
        sspi_instance -> comm_control = temp | SPI_LSB_FIRST ;  
    }
    else
    {
        sspi_instance -> comm_control = temp & (~SPI_LSB_FIRST) ;
    }
}


/** @fn void sspi_enable_txrx(sspi_struct *sspi_instance, uint8_t enable)
 * @brief Enable the sspi to start tx or rx.
 * @details SPI_ENABLE bit in comm_control register is set to 1, SSPI peripheral starts to transmit or receive 
 *          as per configuration
 * @param sspi_instance sspi_instance pointer holds the instance of sspi_struct
 * @param enable If 1, enables the sspi for transmit or receive
 */
void sspi_enable_txrx(sspi_struct *sspi_instance, uint8_t enable)
{
    uint32_t temp = sspi_instance -> comm_control;
    enable &= 0x01;
    if(enable == 1 ){
        sspi_instance -> comm_control = temp | SPI_ENABLE;
    }
    else{
        sspi_instance -> comm_control = temp & ~(SPI_ENABLE);
    }    
}


/** @fn void sspi_configure_tx_setup_time(sspi_struct *sspi_instance, uint8_t delay_value)
 * @brief Configures the sspi setup time.
 * @details Setup time is the Chip select high to transmission delay and holds the number of sspi clock cycle    *          delay before transmission.
 * The required delay will be programmed into the clk register.
 * It takes values from 0 to 255 indicating 0 to 255 SPI clock cycles
 * @param sspi_instance sspi_instance pointer holds the instance of sspi_struct
 * @param delay_value value of delay ranges from 0 to 255
 */
void sspi_configure_tx_setup_time(sspi_struct *sspi_instance, uint8_t delay_value)
{
    uint32_t temp = sspi_instance -> clk_control & ( ~SPI_SS2TX_DELAY(255) );
    sspi_instance -> clk_control = temp |  SPI_SS2TX_DELAY(delay_value);

}


/** @fn void sspi_configure_tx_hold_time(sspi_struct *sspi_instance, uint8_t delay_value) 
 * @brief Configures the sspi hold time
 * @details Hold time is Transmission end to Chip select low and holds the number of sspi clock cycle delay after transmission
 * The required delay will be programmed into the clk register by values ranging from 0 to 255
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param delay_value 
 */
void sspi_configure_tx_hold_time(sspi_struct *sspi_instance, uint8_t delay_value)
{
    uint32_t temp = sspi_instance -> clk_control & (~ SPI_TX2SS_DELAY(255));
    sspi_instance -> clk_control = temp |  SPI_TX2SS_DELAY(delay_value);
}


/** @fn void sspi_configure_clock_in_hz(sspi_struct *sspi_instance, uint32_t bit_rate)
 * @brief Configures the prescaler value for the passed bit_rate.
 * @details Calculates the prescaler value from the passed bit_rate and updates the prescaler value in clock 
 *          control register.
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param bit_rate input is given to calculate the prescaler value 
 */
void sspi_configure_clock_in_hz(sspi_struct *sspi_instance, uint32_t bit_rate)
{
    uint8_t prescaler = (CLOCK_FREQUENCY / bit_rate) - 1;

    if(bit_rate >= CLOCK_FREQUENCY){
        log_error("\n Invalid bit rate value. Bit rate should be less than CLOCK_FREQUENCY");
		return;
    }

    uint32_t temp = sspi_instance -> clk_control & (~SPI_PRESCALE(0xFF) );
    sspi_instance -> clk_control = temp | SPI_PRESCALE(prescaler);
}


/** @fn void sspi_configure_clock_prescaler(sspi_struct *sspi_instance, uint8_t prescaler)
 * @brief Configures the prescaler value for the clock
 * @details Updates the prescaler value in clock control register
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param prescaler Prescaler value takes from 0 to 255 
 */
void sspi_configure_clock_prescaler(sspi_struct *sspi_instance, uint8_t prescaler)
{
    uint32_t temp = sspi_instance -> clk_control & (~SPI_PRESCALE(255));
    sspi_instance -> clk_control = temp | SPI_PRESCALE(prescaler);
    
}


/** @fn void sspi_configure_clock_phase(sspi_struct *sspi_instance, uint8_t clk_phase)
 * @brief Configures the phase value for the clock register
 * @details If phase = 1, sspi transmits or receives data in second clock transition else first clock transition 
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param clk_phase clk_phase takes value 0 or 1
 */
void sspi_configure_clock_phase(sspi_struct *sspi_instance, uint8_t clk_phase)
{
    uint32_t temp = sspi_instance -> clk_control;
    clk_phase &= 0x01;
    if(clk_phase == 1 ){
        sspi_instance -> clk_control = temp | SPI_CLK_PHASE;
    }
    else{
        sspi_instance -> clk_control = temp & ~(SPI_CLK_PHASE);
    }    
}


/** @fn void sspi_configure_clock_pol(sspi_struct *sspi_instance, uint8_t clk_pol)
 * @brief Configures the polarity value for the clock register
 * @details If polarity = 1, default value of the clock is high, else default value of clock line is low
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param clk_pol clk_pol takes value 0 or 1
 */
void sspi_configure_clock_pol(sspi_struct *sspi_instance, uint8_t clk_pol)
{
    uint32_t temp = sspi_instance -> clk_control;
    clk_pol &= 0x01;
    if(clk_pol == 1 ){
        sspi_instance -> clk_control = temp | SPI_CLK_POLARITY;
    }
    else{
        sspi_instance -> clk_control = temp & ~(SPI_CLK_POLARITY);
    }    
}


/** @fn void sspi_configure_clock(sspi_struct *sspi_instance, uint8_t prescaler, uint8_t clk_pol, uint8_t clk_phase)
 * @brief Configures the prescaler, polarity and phase for clock used in sspi communication 
 * @details If polarity = 1, default value of the clock is high, else default value of clock line is low. 
 *          If phase = 1, sspi transmits or receives data in second clock transition else first clock transition
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param prescaler Prescaler value takes from 0 to 255 
 * @param clk_pol clk_polarity takes value 0 or 1
 * @param clk_phase clk_phase takes value 0 or 1
 */
void sspi_configure_clock(sspi_struct *sspi_instance, uint8_t prescaler, uint8_t clk_pol, uint8_t clk_phase)
{
    uint32_t temp = sspi_instance -> clk_control & (~(SPI_PRESCALE(255))) & (~(SPI_CLK_PHASE | SPI_CLK_POLARITY)) ;
    clk_pol &= 0x01;
    clk_phase &= 0x01;
    sspi_instance -> clk_control = temp | SPI_PRESCALE(prescaler) | (clk_pol << 0 ) | (clk_phase << 1);
}

/** @fn void sspi_read_comm_status(sspi_struct *sspi_instance, uint8_t *sts)
 * @brief Read the communication status register  
 * @details  
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param sts pointer which will hold communication status register value 
 */
void sspi_read_comm_status(sspi_struct *sspi_instance, uint8_t *sts)
{
    *sts = sspi_instance -> comm_status;
	printf("\n commn status %x",  *sts);
}

/** @fn void sspi_read_comm_status(sspi_struct *sspi_instance, uint8_t *sts)
 * @brief Read the communication status register  
 * @details  
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param sts pointer which will hold communication status register value 
 */
void sspi_read_registers(sspi_struct *sspi_instance)
{
    printf("\n spi_instance -> comm_control: %x", sspi_instance ->comm_control);
    printf("\n spi_instance -> clk_control: %x", sspi_instance ->clk_control);
    printf("\n spi_instance -> comm_status: %x", sspi_instance ->comm_status);
    printf("\n spi_instance -> fifo_status: %x", sspi_instance ->fifo_status);
    printf("\n spi_instance -> qual: %x", sspi_instance ->qual);
}

/** @fn void sspi_read_fifo_status(sspi_struct *sspi_instance, uint8_t *fifo_sts)
 * @brief Reads the FIFO status register  
 * @details  
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param sts pointer which will hold FIFO status register value 
 */
void sspi_read_fifo_status(sspi_struct *sspi_instance, uint8_t *fifo_sts)
{
    *fifo_sts = sspi_instance -> fifo_status;
}


/** @fn void sspi_check_rx_fifo_full(sspi_struct *sspi_instance, uint8_t *sts)
 * @brief Checking if the Receiver FIFO level is full or not 
 * @details  It checks SPIRXFIFO bits in the Communication status register 
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param sts pointer which will hold 1- if the RX FIFO level is full or else 0. 
 */
void sspi_check_rx_fifo_full(sspi_struct *sspi_instance, uint8_t *sts)
{
    uint8_t temp = (( sspi_instance -> comm_status >> 5) & 0x03);
    if(temp == 3) {
        *sts = 1;
    }
    else{
        *sts = 0;
    }
}


/** @fn void sspi_check_tx_fifo_empty(sspi_struct *sspi_instance, uint8_t *sts)
 * @brief Checking if the tranmitter FIFO level is empty or not 
 * @details  It checks SPITXFIFO bits in the Communication status register 
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param sts pointer which will hold 1- if the TX FIFO level is empty or else 0. 
 */
void sspi_check_tx_fifo_empty(sspi_struct *sspi_instance, uint8_t *sts)
{
    uint8_t temp = ((sspi_instance -> comm_status >> 3) & 0x03);
    if(temp == 0){
        *sts = 1;
    }
    else{
        *sts = 0;
    }
}

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
void sspi_check_tx_fifo_level(sspi_struct *sspi_instance, uint8_t *sts)
{
    *sts = ( (sspi_instance -> comm_status >> 3) & 0x03);

}

/** @fn void sspi_check_rx_fifo_level(sspi_struct *sspi_instance, uint8_t *sts)
 * @brief Checking the reciever FIFO level 
 * @details Reading the RX FIFO level  
 *          00-FIFO empty
 *          01-1/4 FIFO
 *          10-1/2 FIFO
 *          11-FIFO full
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param sts pointer which will hold RX FIFO level. 
 */
void sspi_check_rx_fifo_level(sspi_struct *sspi_instance, uint8_t *sts)
{
    *sts = ( (sspi_instance -> comm_status >> 5) & 0x03);
}

/** @fn void sspi_check_overrun_fifo(sspi_struct *sspi_instance, uint8_t *sts)
 * @brief Checking the overrun error in FIFO  
 * @details Reading SPI_OVR bit in status register  
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param sts pointer which will hold overrun error bit. 
 */
void sspi_check_overrun_fifo(sspi_struct *sspi_instance, uint8_t *sts)
{
    *sts = ( (sspi_instance -> comm_status & SPI_OVR) >> 7 );
}

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
void sspi_enable_tx_rx_interrupts(sspi_struct *sspi_instance, uint8_t tx_int_cntrl, uint8_t rx_int_cntrl)
{
    tx_int_cntrl &= 0xF;
    rx_int_cntrl &= 0xF;
    uint16_t temp = sspi_instance -> intr_en & (~(0xFF));
    sspi_instance ->intr_en = temp | tx_int_cntrl | (rx_int_cntrl << 4);
}

/** @fn void sspi_enable_over_run_interrupt(sspi_struct *sspi_instance, uint8_t over_run_int_cntrl)
 * @brief Enabling the overrun interrupt 
 * @details Enabling the over run interrupt enable bits to get interrupt overrun.
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param over_run_int_cntrl  Holds value from 0 to 1 to set interrupts on SPI_RX_OVERRUN_INTR_EN bit.
 */
void sspi_enable_over_run_interrupt(sspi_struct *sspi_instance, uint8_t overrun_int_cntrl)
{
    uint16_t temp = sspi_instance -> intr_en;
    overrun_int_cntrl &= 0x01;
    if(overrun_int_cntrl == 1 ) {
        sspi_instance -> intr_en = temp | SPI_RX_OVERRUN_INTR_EN;
    }
    else {
        sspi_instance -> intr_en = temp & ~(SPI_RX_OVERRUN_INTR_EN);
    } 
}

/** @fn void sspi_transmit_data(sspi_struct *sspi_instance, uint32_t*buf_data, uint8_t buf_length)
 * @brief To transmit data on TX data register
 * @details Transmitting data by writing buf_data on transmit register after checking the TXE bit on Status register.
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param buf_data this pointer holds the buffer data which has to be transmitted 
 * @param buf_length Holds the length of the buffer data sent
 */
void sspi_transmit_data(sspi_struct *sspi_instance, uint32_t*buf_data, uint8_t buf_length)
{
    uint8_t comm_status  = sspi_instance -> comm_status;
    
    while(buf_length != 0) {
        if(comm_status & SPI_TXE) {
            sspi_instance -> data_tx = *buf_data;
			sspi_enable_txrx(sspi_instance, ENABLE);
	    printf("\n Send data : %x", sspi_instance -> data_tx);
            buf_length --;
        }
    }
}

/** @fn void sspi_receive_data(sspi_struct *sspi_instance, uint32_t *buf_data)
 * @brief Receiving data on RX Data Register
 * @details Receivgin data by reading the RX register and writing the read data on buf_data.
 * @param[in] sspi_instance pointer holds the instance of sspi_struct
 * @param[in] buf_data this pointer holds the received data from the Data RX register 
 * @param[out] Returns success / failure status
 */
int sspi_receive_data(sspi_struct *sspi_instance, uint16_t *buf_data)
{   
	uint32_t timeout = 0;
	uint16_t temp_data = 0;
	while(!(sspi_instance -> comm_status & SPI_RXNE) )
	{
		if(timeout++ > 0x1000) //0x8FFFFFFF)
		{
			printf("\n sspi_instance -> comm_status : %x", sspi_instance -> comm_status );
			return TIMEOUT;
		}
	}
	*buf_data++ = sspi_instance -> data_rx; 
	return SUCCESS;
}

/** @fn void sspi_transmit_data(sspi_struct *sspi_instance, uint32_t*buf_data, uint8_t buf_length)
 * @brief To transmit data on TX data register
 * @details Transmitting data by writing buf_data on transmit register after checking the TXE bit on Status register.
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param buf_data this pointer holds the buffer data which has to be transmitted 
 * @param buf_length Holds the length of the buffer data sent
 */
void sspi_clear_fifo(sspi_struct *sspi_instance)
{
    uint8_t temp_data;
   // sspi_instance->clock_en=SSPI_CLK_EN;
   // log_info("\nclock enable pin ~~%x",sspi_instance->clock_en);
    printf("\n spi clear fifo functiom");
    while(!(sspi_instance -> fifo_status & SPI_TX_EMPTY)) 
    {  printf(".");
        // printf("\n tx not empty");
	    sspi_configure_tx_rx_length(sspi_instance, 8, 8);
	    sspi_configure_comm_mode(sspi_instance, FULL_DUPLEX );
	    sspi_enable_txrx(sspi_instance, ENABLE);
        sspi_notbusy(sspi_instance);
    }
//	printf("\n TX Fifo sspi_instance[SSPI1_INSTANCE]->fifo_status: %x", sspi_instance->fifo_status);
	//printf("\n sspi_instance[SSPI1_INSTANCE]->comm_status: %x", sspi_instance->comm_status);

    while(!(sspi_instance -> fifo_status & SPI_RX_EMPTY))
    {  printf(",");
        // printf("\n rx not empty");
	    temp_data =  sspi_instance->data_rx;				
    }
	printf("\n RX FIFO sspi_instance->fifo_status: %x", sspi_instance->fifo_status);
	printf("\n sspi_instance->comm_status: %x", sspi_instance->comm_status);
    
}


/** @fn unsigned int sspi_receive_n_data(sspi_struct *sspi_instance, uint32_t *buf_data, uint8_t buf_length)
 * @brief Receiving 'n' number of data on RX Data Register
 * @details Receivgin data by reading the RX register and writing the read data on buf_data.
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param buf_data this pointer holds the received data from the Data RX register 
 * @param buf_length points holding number of buffers received
 */
unsigned int sspi_receive_n_data(sspi_struct *sspi_instance, uint32_t *buf_data, uint8_t buf_length)
{   
	uint32_t comm_status = 0;
	uint16_t buf_size = 0;
	uint32_t time_out_count = 0;

	while(buf_length != 0)
	{
		while(!(sspi_instance -> comm_status & SPI_RXNE) )
		{
			if(time_out_count++ > 0x8FFFFFFF)
				return ( (TIMEOUT << 15) | buf_size);
		}
		*buf_data++ = sspi_instance -> data_rx; 
		buf_length--;
		buf_size++;
	}
   return buf_size;
}


/** @fn void sspi_configure_qualification(sspi_struct *sspi_instance, uint8_t prescaler, uint8_t clk_pol, uint8_t clk_phase)
 * @brief Configures the prescaler, polarity and phase for clock used in sspi communication 
 * @details If polarity = 1, default value of the clock is high, else default value of clock line is low. 
 *          If phase = 1, sspi transmits or receives data in second clock transition else first clock transition
 * @param sspi_instance pointer holds the instance of sspi_struct
 * @param prescaler Prescaler value takes from 0 to 255 
 * @param clk_pol clk_polarity takes value 0 or 1
 * @param clk_phase clk_phase takes value 0 or 1
 */
void sspi_configure_qualification(sspi_struct *sspi_instance, uint8_t qual_cycles)
{
    sspi_instance -> qual = qual_cycles;
}


#ifdef INTERRUPT
/** @fn void sspi_isr()
 * @brief Interrupt service routine for SSPI peripheral
 * @details This functions will be called to ofer interrupt service routine for SSPI0
 */
void sspi0_isr_handler()
{
//    sspi_struct *sspi_instance = sspi_instance[0];

    uint32_t comm_status  = sspi_instance[0] -> comm_status;

    if(comm_status & SPI_TXE) {
        sspi_instance[0] -> data_tx = buf_data;
        buf_length --;
    }
    else if(comm_status & SPI_RXNE) {
        buf_data = sspi_instance[0] -> data_rx;
        buf_length ++;
    }
}


/** @fn void sspi_isr()
 * @brief Interrupt service routine for SSPI peripheral
 * @details This functions will be called to ofer interrupt service routine for SSPI0
 */
void sspi1_isr_handler()
{
//    sspi_struct *sspi_instance = sspi_instance[1];

    uint32_t comm_status  = sspi_instance[1] -> comm_status;

    if(comm_status & SPI_TXE) {
        sspi_instance[1] -> data_tx = buf_data;
        buf_length --;
    }
    else if(comm_status & SPI_RXNE) {
        buf_data = sspi_instance[1] -> data_rx;
        buf_length ++;
    }
}
#endif
