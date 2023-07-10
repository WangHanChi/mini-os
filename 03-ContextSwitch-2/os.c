#include <stdint.h>
#include "reg.h"
#include "asm.h"

/* USART TXE Flag
 * This flag is cleared when data is written to USARTx_DR and
 * set when that data is transferred to the TDR
 */
#define USART_FLAG_TXE	((uint16_t) 0x0080)

void usart_init(void)
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
}

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

void usertask(void)
{
	print_str("usertask: 1st call of usertask!\n\r");
	print_str("usertask: Now, return to kernel mode\n\r");
	syscall();
	print_str("usertask: 2nd call of usertask!\n\r");
	print_str("usertask: Now, return to kernel mode\n\r");
	syscall();
	while (1)
		/* wait */ ;
}

int main(void)
{
	/* Initialization of process stack
	 * r4, r5, r6, r7, r8, r9, r10, r11, lr */
	unsigned int usertask_stack[256];
	unsigned int *usertask_stack_start = usertask_stack + 256 - 16;
	usertask_stack_start[8] = (unsigned int) &usertask;

	usart_init();

	print_str("OS: Starting...\n\r");
	print_str("OS: Calling the usertask (1st time)\n\r");
	usertask_stack_start = activate(usertask_stack_start);
	print_str("OS: Return to the OS mode !\n\r");
	print_str("OS: Calling the usertask (2nd time)\n\r");
	usertask_stack_start = activate(usertask_stack_start);
	print_str("OS: Return to the OS mode !\n\r");
	print_str("OS: Going to infinite loop...\n\r");
	while (1)
		/* We can't exit, there is nowhere to go */ ;
	return 0;
}