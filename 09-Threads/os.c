#include <stddef.h>
#include <stdint.h>
#include "reg.h"
#include "thread.h"

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

    while (!(*(USART3_SR) & USART_FLAG_TXE))
        ;
    *(USART3_DR) = '\r';
    /*Waiting for the transmit complete (bit 6 TC)*/
    while (!(*USART3_SR & 0x40))
        ;
}

static void delay(volatile int count)
{
	count *= 5000;
	while (count--);
}

static void busy_loop(void *str)
{
	while (1) {
		print_str(str);
        print_str("hello\n");
		print_str(": Running...\n");
		delay(300);
	}
}

void test1(void *userdata)
{
    print_str("In test1\n");
	busy_loop(userdata);
}

void test2(void *userdata)
{
    print_str("In test2\n");
	busy_loop(userdata);
}

void test3(void *userdata)
{
    print_str("In test3\n");
	busy_loop(userdata);
}

/* 72MHz */
#define CPU_CLOCK_HZ 8000000

/* 100 ms per tick. */
#define TICK_RATE_HZ 1

int main(void)
{
	const char *str1 = "Task1", *str2 = "Task2", *str3 = "Task3";

	usart_init();

	if (thread_create(test1, (void *) str1) == -1)
		print_str("Thread 1 creation failed\r\n");

	if (thread_create(test2, (void *) str2) == -1)
		print_str("Thread 2 creation failed\r\n");

	if (thread_create(test3, (void *) str3) == -1)
		print_str("Thread 3 creation failed\r\n");

	/* SysTick configuration */
	*SYSTICK_RVR = (CPU_CLOCK_HZ / TICK_RATE_HZ) - 1UL;
	*SYSTICK_CVR = 0;
	*SYSTICK_CSR = 0x03;

	thread_start();

	return 0;
}