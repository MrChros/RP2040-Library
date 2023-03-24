/*
 * I2C_Master.c
 *
 * Created: Sun Sep 11 2022 12:00:14
 *  Author Chris
 */

// ============================================================================================
// Includes
#include "I2C_Master.h"

#include "pico/malloc.h"
#include "hardware/i2c.h"
#include "hardware/dma.h"
#include "hardware/irq.h"


// ============================================================================================
// Defines
#define I2C_HARDWARE_UNIT			(&i2c0_inst)
#define I2C_BAUDRATE_HZ				400000UL

#define I2C_SCL_PIN					25
#define I2C_SDA_PIN					24


// ============================================================================================
// Variables
static bool					_DMA_Transfer_Complete;
static int 					_DMA_Transmit_Claim;
static dma_channel_config 	_DMA_Transmit_Channel;


// ============================================================================================
// Function Declarations


/*******************************************************************
	Interrupt Service Routines
*******************************************************************/
void ISR_I2C_DMA_Transmit_Complete()
{
	dma_hw->ints0 = (1u << _DMA_Transmit_Claim);

	_DMA_Transfer_Complete = true;
}


/*******************************************************************
	Functions
*******************************************************************/
void I2CM_Init(bool use_internal_pullups)
{
	i2c_init(I2C_HARDWARE_UNIT, I2C_BAUDRATE_HZ);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
	
	/* Function for internal pull-ups has not been tested yet */
	if(use_internal_pullups == true)
	{
		gpio_pull_up(I2C_SDA_PIN);
    	gpio_pull_up(I2C_SCL_PIN);
	}

	
	// Configura DMA Channel for SPI Transmit function
	_DMA_Transmit_Claim 	= dma_claim_unused_channel(false);
	_DMA_Transmit_Channel 	= dma_channel_get_default_config(_DMA_Transmit_Claim);

	// channel_config_set_transfer_data_size(&_DMA_Transmit_Channel, DMA_SIZE_8);
	channel_config_set_transfer_data_size(&_DMA_Transmit_Channel, DMA_SIZE_16);
	channel_config_set_dreq(&_DMA_Transmit_Channel, i2c_get_dreq(I2C_HARDWARE_UNIT, true));
	channel_config_set_read_increment(&_DMA_Transmit_Channel, true);
  	channel_config_set_write_increment(&_DMA_Transmit_Channel, false);
	dma_channel_configure(	_DMA_Transmit_Claim,
							&_DMA_Transmit_Channel,
							&i2c_get_hw(I2C_HARDWARE_UNIT)->data_cmd,	// Write Address
							NULL, 										// Read Address
							0,											// Element Count (Each element is of size transfer_data_size)
							false);	 									// DO NOT start directly


	dma_channel_set_irq1_enabled(_DMA_Transmit_Claim, true);

    // Configure the processor to run the ISR when DMA IRQ 0 is asserted
    irq_set_exclusive_handler(DMA_IRQ_1, ISR_I2C_DMA_Transmit_Complete);
    irq_set_enabled(DMA_IRQ_1, true);

	_DMA_Transfer_Complete = true;
}

int I2CM_Transmit(uint8_t slave_address, void *data, size_t data_length, bool use_dma, bool nostop)
{
	if(use_dma)
	{
		// return 0;
		if(dma_channel_is_busy(_DMA_Transmit_Claim)) {
			return 0;
		}

		// Apply flag for Stop Bit at last Data Set
		((uint16_t*)data)[data_length-1] |= I2C_IC_DATA_CMD_STOP_BITS;

		dma_channel_set_read_addr	(_DMA_Transmit_Claim, (uint16_t*) data	, false);
		dma_channel_set_trans_count	(_DMA_Transmit_Claim, data_length		, true);
	}
	else
	{
		return i2c_write_blocking(I2C_HARDWARE_UNIT, slave_address, (uint8_t*)data, data_length, nostop);
	}
}

int I2CM_Receive(uint8_t slave_address, uint8_t *receive_data, size_t data_length)
{
	return i2c_read_blocking(I2C_HARDWARE_UNIT, slave_address, receive_data, data_length, false);
}

bool I2CM_DMA_Transmit_Complete()
{
	bool Return_Value = _DMA_Transfer_Complete;

	_DMA_Transfer_Complete = false;

	return Return_Value;
}

int I2CM_Packet_Transmit(uint8_t slave_address, uint reg_address, uint8_t address_length, uint8_t *transmit_data, uint8_t data_length)
{
	uint8_t *Transmit_Data = malloc(address_length + data_length);
	
	for(uint i=0;i<address_length;i++)
	{
		Transmit_Data[i] = (uint8_t)(reg_address >> (8*(address_length - i - 1)));
	}

	for(uint i=0;i<data_length;i++)
	{
		Transmit_Data[i + address_length] = transmit_data[i];
	}

	int status = I2CM_Transmit(slave_address, Transmit_Data, address_length + data_length, false, false);

	free(Transmit_Data);

	return status;
}

// Receive packet from specific slave address
int I2CM_Packet_Receive(const uint8_t slave_address, const uint reg_address, const uint8_t address_length, uint8_t *receive_data , const uint8_t data_length)
{
	uint8_t *Address_Data = malloc(address_length);
	for(uint i=0;i<address_length;i++)
	{
		Address_Data[i] = (uint8_t)(reg_address >> (8*(address_length - i - 1)));
	}
	
	int status = I2CM_Transmit(slave_address, Address_Data, address_length, false, true);

	return I2CM_Receive(slave_address, receive_data, data_length);
}