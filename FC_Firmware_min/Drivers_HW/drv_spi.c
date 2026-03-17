#include "drv_spi.h"

uint8_t SPI_Read_Register(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin, uint8_t reg_addr){
	uint8_t tx_data[2] = {reg_addr | 0x80, 0x00};
	uint8_t rx_data[2] = {0};


	HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET); //keo chan cs xuong low goi cam bien
	HAL_SPI_TransmitReceive(hspi, tx_data, rx_data, 2, 100); //truyen va nhan du lieu
	HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET); //keo CS leen High ket thuc


	return rx_data[1];

}

void SPI_Write_Register(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin, uint8_t reg_addr, uint8_t data){
	uint8_t tx_data[2] = {reg_addr & 0x7F, data}; 	//Bit cao nhat = 0 de bao la Write


	HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(hspi, tx_data, 2, 100);
	HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);

}
