/*
 * Display_SPI.c
 *
 * Created: Sun Mar 21 2021 11:51:35
 *  Author Chris
 */
#include "Display_SPI.h"
#include "Display_Config.h"

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "pico/binary_info.h"
#include "hardware/structs/spi.h"


// ============================================================================================
// Defines


// ============================================================================================
// Variables
static uint					_Baudrate;
static bool					_SPI_Received_Initialized;
volatile bool				_DMA_Transfer_Complete;
static uint 				_DMA_Transmit_Claim;
static dma_channel_config 	_DMA_Transmit_Channel;


// ============================================================================================
// Function Declarations
static inline void Display_SPI_CS_Select();
static inline void Display_SPI_CS_Deselect();


/*******************************************************************
	Interrupt Service Routines
*******************************************************************/
void ISR_SPI_DMA_Transmit_Complete()
{
	dma_hw->ints0 = (1u << _DMA_Transmit_Claim);
	Display_SPI_CS_Deselect();

	_DMA_Transfer_Complete = true;
}

/*******************************************************************
	Functions
*******************************************************************/
void Display_SPI_Init(bool init_spi_receive)
{
	_SPI_Received_Initialized = init_spi_receive;
	
	_Baudrate = spi_init(DISPLAY_SPI_PORT, DISPLAY_SPI_BAUDRATE);

	// Initialize SPI Pinss defined in Config-Struct
	gpio_set_slew_rate	(DISPLAY_SPI_PIN_SCK, GPIO_SLEW_RATE_FAST);
	gpio_set_slew_rate	(DISPLAY_SPI_PIN_MOSI, GPIO_SLEW_RATE_FAST);
    gpio_set_function	(DISPLAY_SPI_PIN_SCK, 	GPIO_FUNC_SPI);
    gpio_set_function	(DISPLAY_SPI_PIN_MOSI, GPIO_FUNC_SPI);
	
	if(_SPI_Received_Initialized == true) {
    	gpio_set_function(DISPLAY_SPI_PIN_MISO, GPIO_FUNC_SPI);
	}

	// Initialize CS-Pin as regular GPIO
	gpio_init				(DISPLAY_SPI_PIN_CS);
    gpio_set_dir			(DISPLAY_SPI_PIN_CS, GPIO_OUT);
	gpio_set_slew_rate		(DISPLAY_SPI_PIN_CS, GPIO_SLEW_RATE_FAST);

    Display_SPI_CS_Deselect();


	// Configura DMA Channel for SPI Transmit function
	_DMA_Transmit_Claim 	= dma_claim_unused_channel(true);
	_DMA_Transmit_Channel 	= dma_channel_get_default_config(_DMA_Transmit_Claim);
	channel_config_set_transfer_data_size(&_DMA_Transmit_Channel, DMA_SIZE_8);
	channel_config_set_dreq(&_DMA_Transmit_Channel, spi_get_index(DISPLAY_SPI_PORT) ? DREQ_SPI1_TX : DREQ_SPI0_TX);
	dma_channel_configure(	_DMA_Transmit_Claim,
							&_DMA_Transmit_Channel,
							&spi_get_hw(DISPLAY_SPI_PORT)->dr, 	// Write Address
							NULL, 								// Read Address
							0,									// Element Count (Each element is of size transfer_data_size)
							false);	 							// DO NOT start directly


	dma_channel_set_irq0_enabled(_DMA_Transmit_Claim, true);

    // Configure the processor to run the ISR when DMA IRQ 0 is asserted
    irq_set_exclusive_handler(DMA_IRQ_0, ISR_SPI_DMA_Transmit_Complete);
    irq_set_enabled(DMA_IRQ_0, true);

	_DMA_Transfer_Complete = true;
}

void Display_SPI_Start_Command(uint8_t command)
{
	DISPLAY_MODE_SET_CMD();
	Display_SPI_CS_Select();
	spi_write_blocking(DISPLAY_SPI_PORT, &command, 1);
	DISPLAY_MODE_SET_DATA();
}

void Display_SPI_Start_Parameter(uint8_t parameter)
{
	spi_write_blocking(DISPLAY_SPI_PORT, &parameter, 1);
}

void Display_SPI_Start_Parameter_Array(uint8_t* parameter_array, uint length)
{
	spi_write_blocking(DISPLAY_SPI_PORT, parameter_array, length);
}

void Display_SPI_Finish_Command(void)
{
	Display_SPI_CS_Deselect();
}

void Display_SPI_Send_Data(uint8_t* data, uint length, bool use_dma)
{
	Display_SPI_CS_Select();
	if(use_dma) {
		if(dma_channel_is_busy(_DMA_Transmit_Claim)) {
			return;
		}

		dma_channel_set_read_addr	(_DMA_Transmit_Claim, data	, false);
		dma_channel_set_trans_count	(_DMA_Transmit_Claim, length, true);
	} else {
		spi_write_blocking(DISPLAY_SPI_PORT, data, length);
		Display_SPI_CS_Deselect();
	}
}

void Display_SPI_Set_Baudrate(uint baudrate)
{
	_Baudrate = spi_init(DISPLAY_SPI_PORT, baudrate);	// This works when changing the Baudrate during runtime
														// Using the function 'spi_set_baudrate' bring some problems
	// _Baudrate = spi_set_baudrate(DISPLAY_SPI_PORT, baudrate);
}

uint Display_SPI_Get_Baudrate(void)
{
	return _Baudrate;
}

bool Display_SPI_DMA_Transfer_Ongoing(void)
{
	return dma_channel_is_busy(_DMA_Transmit_Claim);
}

bool Display_SPI_DMA_Transfer_Completed(void)
{
	bool Return_Value = _DMA_Transfer_Complete;

	_DMA_Transfer_Complete = false;

	return Return_Value;
}

bool Display_SPI_Read_Write(const uint8_t* write_data, uint8_t* read_data, uint length_both_buffers)
{
	if(_SPI_Received_Initialized == false) {
		return false;
	}

	int Bytes_Read_Write;
	Bytes_Read_Write = spi_write_read_blocking(DISPLAY_SPI_PORT, write_data, read_data, length_both_buffers);

	return (Bytes_Read_Write == length_both_buffers);
}

/*******************************************************************
	Internal Functions
*******************************************************************/
static inline void Display_SPI_CS_Select() 
{
    asm volatile("nop \n nop \n nop");
    gpio_put(DISPLAY_SPI_PIN_CS, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void Display_SPI_CS_Deselect()
{
    asm volatile("nop \n nop \n nop");
    gpio_put(DISPLAY_SPI_PIN_CS, 1);
    asm volatile("nop \n nop \n nop");
}