#include <stdint.h>
#include "reg.h"

/* USART TXE Flag
 * This flag is cleared when data is written to USARTx_DR and
 * set when that data is transferred to the TDR
 */
#define USART_FLAG_TXE ((uint16_t) 0x0080)

/* USART RXNE Flag
 * This flag is set to 1 means "Received data is ready to be read."
 * Set to 0 means "Data is not received"
 */
#define USART_FLAG_RXNE ((uint16_t) 0x0020)

/* greet is a global variable
 * This variable will be load by the loader at LMA and will be
 * copy to VMA by startup.c during startup.
 *
 * Add a global variable greet instead of a string literal
 * because string literal is in .rodata region and is put
 * under the .text region by the linker script
 */
static char greet[1000] = "This is simple CLI !\n\r";

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

void scan_str(char *str)
{
    char received_char;
    int index = 0;

    while (1)
    {
        /* Wait until a character is received (USART_RXNE flag is set) */
        while (!(*USART3_SR & USART_FLAG_RXNE))
            ;

        /* Read the received character from USART3_DR */
        received_char = *USART3_DR & 0xFF;

        /* Clear RXNE by set 0 */
        *USART3_SR &= ~(USART_FLAG_RXNE);

        /* Check if it is the end of line (Enter key) */
        if (received_char == '\r')
        {
            /* Null-terminate the string and exit the loop */
            str[index] = '\0';
            break;
        }

        /* Store the received character in the buffer */
        str[index] = received_char;

        /* Increment the index for the next character */
        index++;
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
    {
        print_str("Enter >");
        scan_str(greet);
        print_str("\n\rYou enter is >> ");
        print_str(greet);
        print_str("\n\n\r");
    }
    
	return 0;
}