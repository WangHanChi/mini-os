#include <stdint.h>
#include "reg.h"

/* USART TXE Flag
 * This flag is cleared when data is written to USARTx_DR and
 * set when that data is transferred to the TDR
 */
#define USART_FLAG_TXE ((uint16_t) 0x0080)

/* greet is a global variable
 * This variable will be load by the loader at LMA and will be
 * copy to VMA by startup.c during startup.
 *
 * Add a global variable greet instead of a string literal
 * because string literal is in .rodata region and is put
 * under the .text region by the linker script
 */
static char greet[] = "123Hello World!\n\r";

void print_str(const char *str)
{
    while (*str) {
        while (!(*(USART3_SR) &USART_FLAG_TXE))
            ;
        *(USART3_DR) = (*str++ & 0xFF);
		/*Waiting for the transmit complete (bit 6 TC)*/
        while (!(*USART3_SR & 0x40))
            ;
    }
}

int main(void)
{
    *(RCC_AHB1ENR)      |= (uint32_t) (0x01 << 3);
    *(RCC_APB1ENR)      |= (uint32_t) (0x01 << 18);

    /* USART3 Configuration, Rx->PD8, Tx->PD9 */
    *(GPIOD)            &= (uint32_t) ~(0x0F << 16);
    *(GPIOD)            |= (uint32_t) (0x02 << 16);
    *(GPIOD)            |= (uint32_t) (0x02 << 18);
    *(GPIOD_AFRH)       &= (uint32_t) ~(0xFF << 0);
    *(GPIOD_AFRH)       |= (uint32_t) (0x77 << 0);

    *(USART3_CR1)       |= (0x01 << 2);
    *(USART3_CR1)       |= (0x01 << 3);
    *(USART3_CR1)       |= (0x01 << 13);
    *(USART3_BRR)       = DEFAULT_F_CLK / BAUDRATE_38400;

    print_str(greet);

    while (1)
        ;
	return 0;
}