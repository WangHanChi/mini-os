/*
 * myusart.h
 *
 *  Created on: 2023年2月8日
 *      Author: hanchi
 */

#ifndef MYUSART_H_
#define MYUSART_H_
#include <stdint.h>

/*板子的頻率*/
#define DEFAULT_F_CLK	16000000U
/*板子鮑率*/
#define BAUDRATE_38400      38400U

/*RCC 基地址*/
#define RCC_BASEADDRESS 	*(volatile uint32_t*)0x40023800U

///// AHB1 /////
/*AHB1 offset*/
#define AHB1_ENR_OFFSET 	*(volatile uint32_t*)0x30U
/*RCC_AHB1ENR*/
#define RCC_AHB1ENR			*(volatile uint32_t*)((0x40023800U) + (0x30U))

///// APB1 /////
/*APB1 offset*/
#define APB1_ENR_OFFSET 	*(volatile uint32_t*)0x40U
/*RCC_APB1ENR*/
#define RCC_APB1ENR			*(volatile uint32_t*)((0x40023800U) + (0x40U))

///// GPIOD /////
/*GPIOD base address*/
#define GPIOD_BASEADDRESS	*(volatile uint32_t*)0x40020c00U
/*GPIOD offset*/
#define GPIOD_OFFSET		*(volatile uint32_t*)0x0U
/*GPIOD_MODE*/
#define GPIOD_MODE			*(volatile uint32_t*)((0x40020c00U) + (0x0U))
/*GPIOD alternate function high register offset*/
#define GPIOD_AFRH_OFFSET	*(volatile uint32_t*)0x24
/*GPIOD alternate function high register*/
#define GPIOD_AFRH			*(volatile uint32_t*)((0x40020c00U)+(0x24))


///// USART3 /////

/*Status register (USART SR)*/
#define USART3_SR			*(volatile uint32_t*)(0x40004800)
/*data register (USART DR)*/
#define USART3_DR			*(volatile uint32_t*)(0x40004800 + 0x04)
/*Baud rate register (USART BRR)*/
#define USART3_BRR			*(volatile uint32_t*)(0x40004800 + 0x08)
/*Control register 1 (USART CR1)*/
#define USART3_CR1 			*(volatile uint32_t*)(0x40004800 + 0x0C)
/*Control register 2 (USART CR2)*/
#define USART3_CR2			*(volatile uint32_t*)(0x40004800 + 0x10)
/*Contorl register 3 (USART CR3)*/
#define USART3_CR3			*(volatile uint32_t*)(0x40004800 + 0x14)

/////  FUNCTION /////
void MYUSART_Init();
void MYUSART_SendData(uint8_t* pTxBuffer, uint8_t len);
uint8_t MYUSART_ReceiveData();

#endif /* MYUSART_H_ */