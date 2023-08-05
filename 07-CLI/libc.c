#include "reg.h"
#include "libc.h"

#define NULL 0

/* This file is to copy the function in glibc and
 * implement for the stm32f429 board. */


/* print the char, similar to 'putchar' */
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

/* scan the stdin for user keyin */
void scan_str(char *str)
{
    char received_char;
    int index = 0;

    /* Clear input buffer */
    while ((*USART3_SR & USART_FLAG_RXNE))
    {
        received_char = *USART3_DR & 0xFF;
    }

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

/* enable the usart for the communication with host */
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

/* Get string lebgth */
int strlen(const char *str)
{
    int i = 0;
    while (str[i])
        i++;
    return i;
}

/* wrap print_str to printf */
void myprintf(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '%') {
            format++;
            if (*format == '\0') break;

            if (*format == 'c') {
                char c = va_arg(args, int); // char type is promoted to int in va_arg
                print_char(c);
            } else if (*format == 's') {
                char *s = va_arg(args, char*);
                print_str(s);
            } else if (*format == 'd') {
                int num = va_arg(args, int);
                char buffer[20]; // adjust buffer size as needed
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

/* Function to emulate sscanf for a character array */
int sscanf_str(const char *input_str, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    int num_of_matched_items = 0;

    while (*format != '\0')
    {
        if (*format == '%')
        {
            format++; // Move past '%'

            // Check the format specifier
            switch (*format)
            {
                case 'd':
                {
                    int *value = va_arg(args, int *);

                    // Parse the integer directly from input_str
                    int parsed_value = 0;
                    while (*input_str >= '0' && *input_str <= '9')
                    {
                        parsed_value = parsed_value * 10 + (*input_str - '0');
                        input_str++;
                    }

                    *value = parsed_value;
                    num_of_matched_items++;
                    break;
                }
                case 's':
                {
                    char *value = va_arg(args, char *);
                    int index = 0;

                    while (*input_str != '\0' && *input_str != ' ' && *input_str != '\t')
                    {
                        value[index] = *input_str;
                        input_str++;
                        index++;
                    }

                    value[index] = '\0';
                    num_of_matched_items++;
                    break;
                }
                case '*':
                {
                    // Skip the input for %*s, but don't store it anywhere
                    while (*input_str != '\0' && *input_str != ' ' && *input_str != '\t')
                    {
                        input_str++;
                    }
                    break;
                }
                // Add more cases for other format specifiers if needed
                default:
                {
                    // Unsupported format specifier
                    va_end(args);
                    return num_of_matched_items;
                }
            }
        }
        else if (*format == ' ')
        {
            // Skip spaces in the format string
            while (*input_str == ' ' || *input_str == '\t')
                input_str++;
        }
        else
        {
            // Non-format characters should match exactly
            if (*input_str != *format)
            {
                va_end(args);
                return num_of_matched_items;
            }
            input_str++;
        }

        format++;
    }

    va_end(args);
    return num_of_matched_items;
}

char *memcpy(char *dest, char *src, int count)
{
    if (count > 0) {
        do {
            count--;
            dest[count] = src[count];
        } while (count > 0);
    }
    return dest;
}

int strcmp(const char *s1, const char *s2)
{
    int i = 0;
    while (s1[i] && s2[i]) {
        if (s1[i] < s2[i])
            return -1;
        else if (s1[i] > s2[i])
            return 1;
        i++;
    }
    return s1[i] - s2[i];
}

int strncmp(char *s1, char *s2, int len)
{
    int i = 0;
    while (i < len) {
        if (s1[i] < s2[i])
            return -1;
        else if (s1[i] > s2[i])
            return 1;
        i++;
    }
    return 0;
}

char *strcpy(char *dest, const char *src)
{
    int i = 0;
    while (src[i]) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = 0;
    return dest;
}

char *strncpy(char *dest, char *src, int len)
{
    int i = 0;
    int beyond = 0;
    while (i < len) {
        if (beyond == 0) {
            dest[i] = src[i];
            if (src[i] == 0)
                beyond = 1;
        } else {
            dest[i] = 0;
        }
        i++;
    }
    return dest;
}

char *strcat(char *dest, const char *src)
{
    if (dest == NULL || src == NULL){
		return NULL;
	}
	
	char *tmp=dest;       
	while(*dest	!= '\0'){
		dest++;
	}

	while(*src != '\0'){
		*dest++ = *src++;
	}
	*dest = '\0';
	return tmp; 
}

void flush()
{
    char received_char;
    // Clear input buffer
    while ((*USART3_SR & USART_FLAG_RXNE)){
        received_char = *USART3_DR & 0xFF;
    }
    (void)received_char;
}
