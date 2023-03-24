/*
 * I2C_Master.h
 *
 * Created: 22/01/2020 18:36:25
 *  Author: Chris
 */ 
#ifndef I2C_MASTER_H_
#define I2C_MASTER_H_

// ============================================================================================
// Includes
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#include "pico/types.h"
#include "pico/stdlib.h"


// ============================================================================================
// Defines
#define I2C_USE_INTERNAL_PULLUP		true
#define I2C_NO_INTERNAL_PULLUP		false


// ============================================================================================
// Function Declarations
void I2CM_Init(bool use_internal_pullups);

int I2CM_Transmit(uint8_t slave_address, void *data, size_t data_length, bool use_dma, bool nostop);
int I2CM_Receive(uint8_t slave_address, uint8_t *receive_data, size_t data_length);
bool I2CM_DMA_Transmit_Complete();

int I2CM_Packet_Transmit(const uint8_t slave_address, const uint reg_address, const uint8_t address_length, uint8_t *transmit_data, const uint8_t data_length);
int I2CM_Packet_Receive (const uint8_t slave_address, const uint reg_address, const uint8_t address_length, uint8_t *receive_data , const uint8_t data_length);

void I2CM_Set_Address(const uint8_t Address);

#endif // I2C_MASTER_H_