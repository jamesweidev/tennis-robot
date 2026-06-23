/*
 * lcd.c
 *
 *  Created on: May 25, 2026
 *      Author: james
 */

#include "lcd.h"

//static void write_nibble(uint8_t data, uint8_t is_data_write)
//{
//	// Configure RS pin
//	if (is_data_write)
//	{
//		GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_PIN_RS, 1);
//	} else
//	{
//		GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_PIN_RS, 0);
//	}
//
//
//	// Start configuring pin values
//	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_PIN_DB4, (data >> 0) & 1); // Configure DB4
//	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_PIN_DB5, (data >> 1) & 1); // Configure DB5
//	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_PIN_DB6, (data >> 2) & 1); // Configure DB6
//	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_PIN_DB7, (data >> 3) & 1); // Configure DB7
//
//	// Toggle E
//	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_PIN_E, 1);
//	delay(1);
//	GPIO_WriteToOutputPin(LCD_GPIO_PORT, LCD_PIN_E, 0);
//}
//
//static void write_command(uint8_t cmd)
//{
//	write_nibble((cmd >> 4) & 0xF, LCD_INST_WRITE);
//	write_nibble(cmd & 0xF, LCD_INST_WRITE);
//	delay(2);
//}
//
//static void write_byte(uint8_t data)
//{
//	write_nibble((data >> 4) & 0xF, LCD_DATA_WRITE);
//	write_nibble(data & 0xF, LCD_DATA_WRITE);
//	delay(1);
//}
//
//void lcd_init()
//{
//	// GPIO inits
//	GPIO_Handle_t gpio_handle;
//	GPIO_Config_t gpio_config;
//
//	gpio_handle.pGPIOx = LCD_GPIO_PORT;
//
//	gpio_config.GPIO_PinAltFuncMode = 0;
//	gpio_config.GPIO_PinMode = GPIO_PIN_MODE_OUT;
//	gpio_config.GPIO_PinOPType = GPIO_OT_PP;
//	gpio_config.GPIO_PinPuPdControl = GPIO_PUPD_NONE;
//	gpio_config.GPIO_PinSpeed = GPIO_OSPEED_FAST;
//
//	gpio_config.GPIO_PinNumber = LCD_PIN_SCL;
//	gpio_handle.GPIOConfig = gpio_config;
//	GPIO_Init(&gpio_handle);
//
//	gpio_config.GPIO_PinNumber = LCD_PIN_SDA;
//	gpio_handle.GPIOConfig = gpio_config;
//	GPIO_Init(&gpio_handle);
//
//	// Initialization Sequence
//	delay(40);
//
//	write_nibble(0x3, LCD_INST_WRITE);
//	delay(5);
//	write_nibble(0x3, LCD_INST_WRITE);
//	delay(1);
//	write_nibble(0x3, LCD_INST_WRITE);
//	delay_us(100);
//
//	write_nibble(0b0010, LCD_INST_WRITE);
//	delay(1);
//
//	write_command(0x28);
//	write_command(0x08);
//	write_command(0x01);
//	delay(2);
//	write_command(0x06);
//	write_command(0x0C);
//}
//
//
//
//uint8_t lcd_write(uint8_t* dataBuffer, uint8_t len)
//{
//	if (len > (16 * 2))
//	{
//		return 1;
//	}
//
//
//	while (len > 0)
//	{
//		write_byte(*dataBuffer);
//
//		len--;
//		dataBuffer++;
//	}
//
//	return 0;
//}
//
//void lcd_clear(void)
//{
//	write_command(0x01);
//}


