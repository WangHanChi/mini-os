#include <stdarg.h>
#include "os.h"

/* USART TXE Flag
 * This flag is cleared when data is written to USARTx_DR and
 * set when that data is transferred to the TDR
 */
#define USART_FLAG_TXE ((uint16_t) 0x0080)

extern uint32_t DEFAULT_F_CLK;
extern uint32_t BAUDRATE;

void print_char(char c)
{
    while (!(*USART3_SR & USART_FLAG_TXE))
        ;
    *USART3_DR = c;
    while (!(*USART3_SR & 0x40))
        ;
}

/* use print_char to warpper puts */
void print_str(const char *str)
{
    while (*str) {
        print_char(*str);
        str++;
    }
}

void myprintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '%') {
            format++;
            if (*format == '\0')
                break;

            if (*format == 'c') {
                char c = va_arg(args,
                                int);  // char type is promoted to int in va_arg
                print_char(c);
            } else if (*format == 's') {
                char *s = va_arg(args, char *);
                print_str(s);
            } else if (*format == 'd') {
                int num = va_arg(args, int);
                char buffer[20];  // adjust buffer size as needed
                int i = 0;
                if (num < 0) {
                    print_char('-');
                    num = -num;
                }
                if (num == 0) {
                    print_char('0');
                } else {
                    while (num > 0) {
                        buffer[i++] = (num % 10) + '0';
                        num /= 10;
                    }
                    while (i > 0) {
                        i--;
                        print_char(buffer[i]);
                    }
                }
            } else {
                // Unsupported format specifier
                print_char(*format);
            }
        } else {
            print_char(*format);
        }
        format++;
    }
    va_end(args);
}

void usart_init(void)
{
    *(RCC_AHB1ENR) |= (uint32_t) (0x01 << 3);
    *(RCC_APB1ENR) |= (uint32_t) (0x01 << 18);

    /* USART3 Configuration, Rx->PD8, Tx->PD9 */
    *(GPIOD) &= (uint32_t) ~(0x0F << 16);
    *(GPIOD) |= (uint32_t) (0x02 << 16);
    *(GPIOD) |= (uint32_t) (0x02 << 18);
    *(GPIOD_AFRH) &= (uint32_t) ~(0xFF << 0);
    *(GPIOD_AFRH) |= (uint32_t) (0x77 << 0);

    *(USART3_CR1) |= (0x01 << 2);
    *(USART3_CR1) |= (0x01 << 3);
    *(USART3_CR1) |= (0x01 << 13);
    *(USART3_BRR) = DEFAULT_F_CLK / BAUDRATE;
}

void delay(volatile int count)
{
    count *= 10000;
    while (count--)
        ;
}

void strcpy(char *dest, const char *src)
{
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}