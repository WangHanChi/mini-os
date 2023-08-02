#ifndef __REG_H_
#define __REG_H_

#include <stdint.h>

#define __REG_TYPE volatile uint32_t
#define __REG __REG_TYPE *

/* RCC Memory Map */
#define RCC             ((__REG_TYPE) 0x40023800)
#define RCC_CR          ((__REG) (RCC + 0x00))
#define RCC_PLLCFGR     ((__REG) (RCC + 0x04))
#define RCC_CFGR        ((__REG) (RCC + 0x08))
#define RCC_CIR         ((__REG) (RCC + 0x0C))
#define RCC_AHB1RSTR    ((__REG) (RCC + 0x10))
#define RCC_AHB2RSTR    ((__REG) (RCC + 0x14))
#define RCC_APB1RSTR    ((__REG) (RCC + 0x20))
#define RCC_APB2RSTR    ((__REG) (RCC + 0x24))
#define RCC_AHB1ENR     ((__REG) (RCC + 0x30))
#define RCC_AHB2ENR     ((__REG) (RCC + 0x34))
#define RCC_APB1ENR     ((__REG) (RCC + 0x40))
#define RCC_APB2ENR     ((__REG) (RCC + 0x44))
#define RCC_BDCR        ((__REG) (RCC + 0x70))
#define RCC_CSR         ((__REG) (RCC + 0x74))

/* Flash Memory Map */
#define FLASH           ((__REG_TYPE) 0x40023C00)
#define FLASH_ACR       ((__REG) (FLASH + 0x00))

/* GPIO Memory Map */
#define GPIOD_BASE      ((__REG_TYPE) 0x40020C00)
#define GPIOD           ((__REG) (GPIOD_BASE + 0x00))
#define GPIOD_IDR       ((__REG) (GPIOD_BASE + 0x10))
#define GPIOD_ODR       ((__REG) (GPIOD_BASE + 0x14))
#define GPIOD_BSRR      ((__REG) (GPIOD_BASE + 0x18))
#define GPIOD_LCKR      ((__REG) (GPIOD_BASE + 0x1C))
#define GPIOD_AFRL      ((__REG) (GPIOD_BASE + 0x20))
#define GPIOD_AFRH      ((__REG) (GPIOD_BASE + 0x24))

/* USART3 Memory Map */
#define USART3          ((__REG_TYPE) 0x40004800)
#define USART3_SR       ((__REG) (USART3 + 0x00))
#define USART3_DR       ((__REG) (USART3 + 0x04))
#define USART3_BRR      ((__REG) (USART3 + 0x08))
#define USART3_CR1      ((__REG) (USART3 + 0x0C))
#define USART3_CR2      ((__REG) (USART3 + 0x10))
#define USART3_CR3      ((__REG) (USART3 + 0x14))
#define USART3_GTPR     ((__REG) (USART3 + 0x18))

/* frequency of stm32 board */
#define DEFAULT_F_CLK 8000000U
#define BAUDRATE_38400 38400U

#endif