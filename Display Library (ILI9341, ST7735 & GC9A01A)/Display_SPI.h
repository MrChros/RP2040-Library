/*
 * Display_SPI.h
 *
 * Created: Sun Mar 21 2021 11:51:32
 *  Author Chris
 */
#ifndef DISPLAY_SPI_H_
#define DISPLAY_SPI_H_

// ============================================================================================
// Includes
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "pico/types.h"
#include "hardware/spi.h"
#include "Display_Config.h"


// ============================================================================================
// Defines


// ============================================================================================
// Datatypes


// ============================================================================================
// Function Declarations
void Display_SPI_Init(bool init_spi_receive);

void Display_SPI_Start_Command(uint8_t command);
void Display_SPI_Start_Parameter(uint8_t parameter);
void Display_SPI_Start_Parameter_Array(uint8_t* parameter_array, uint length);
void Display_SPI_Finish_Command(void);

void Display_SPI_Send_Data(uint8_t* data, uint length, bool use_dma);

void Display_SPI_Set_Baudrate(uint baudrate);
uint Display_SPI_Get_Baudrate(void);
bool Display_SPI_DMA_Transfer_Ongoing(void);
bool Display_SPI_DMA_Transfer_Completed(void);

bool Display_SPI_Read_Write(const uint8_t* write_data, uint8_t* read_data, uint length_both_buffers);

#endif /* DISPLAY_SPI_H_ */