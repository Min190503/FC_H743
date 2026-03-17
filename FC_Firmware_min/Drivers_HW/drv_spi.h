#ifndef DRV_SPI_H_
#define DRV_SPI_H_

#include "main.h"

uint8_t SPI_Read_Register(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin, uint8_t reg_addr);
void SPI_Write_Register(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin, uint8_t reg_addr, uint8_t data);

#endif
