/*
 *  Filename   : myusart.c
 *  Created on : 2023/07/06
 *  Author     : WangHanChi
 */

#include "../Inc/myusart.h"
#include <stdint.h>
#include <stdio.h>

void MYUSART_Init()
{
    ///// GPIO Set /////
    /*GPIOD RCC Enable*/
    RCC_AHB1ENR |= (0x01 << 3); /*Enable GPIOD*/
    /*GPIOD Mode Set*/
    GPIOD_MODE &= ~(0xF << 16); /*Clear PD8 PD9*/
    GPIOD_MODE |=
        (0x2 << 16) | (0x2 << 18); /*PD8 PD9 set alternate function mode*/
    /*GPIOD Alternate Function Set*/
    GPIOD_AFRH &= ~(0xFF << 0); /*Clear AFR8 AFR9*/
    GPIOD_AFRH |= (0x77 << 0);  /*PD8 : USART3 Tx  PD9 : USART3 Rx*/

    ///// USART Set /////
    /*USART RCC Enable*/
    RCC_APB1ENR |= (1 << 18); /*Enable clock of USART3*/
    /*UE (USART Enable)*/
    USART3_CR1 |= (1 << 13); /*Set UE = 1*/
    /*USART Mode set*/
    USART3_CR1 |= (1 << 2) | (1 << 3); /*Enable Tx/Rx*/
    /*Baudrate Set*/
    USART3_BRR = DEFAULT_F_CLK / BAUDRATE_38400;
}

void MYUSART_SendData(uint8_t *pTxBuffer, uint8_t len)
{
    for (uint8_t i = 0; i < len; ++i) {
        /*Waiting for the transmit data register empty (bit 7 TXE)*/
        while (!(USART3_SR & 0x80))
            ;
        /*Send data*/
        USART3_DR = (*pTxBuffer++ & 0XFF);
        /*Waiting for the transmit complete (bit 6 TC)*/
        while (!(USART3_SR & 0x40))
            ;
    }
}

uint8_t MYUSART_ReceiveData()
{
    uint8_t data;
    /*Waiting for the transmit data transmit to USART_RDR register (USART_ISR
     * bit 5 RXNE)*/
    while (!(USART3_SR & 0x20))
        ;
    /*Receive data*/
    data = USART3_DR;
    /*Clear RXNE by set 0*/
    USART3_SR &= ~(0x1 << 5);
    return data;
}

int _write(int file, char *ptr, int len)
{
    MYUSART_SendData((uint8_t *) ptr, len);
    MYUSART_SendData((uint8_t *) "\r", 1);
    return len;
}

int _read(int file, char *ptr, int len)
{
    for (int i = 0; i < len; i++) {
        *ptr = (char) MYUSART_ReceiveData();
        if (*ptr == '\r')
            break; /* read Enter */
        MYUSART_SendData((uint8_t *) ptr++, 1);
    }
    MYUSART_SendData((uint8_t *) "\n\r", 2);
    return len;
}
