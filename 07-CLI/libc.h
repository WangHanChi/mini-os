#ifndef __LIBC_H_
#define __LIBC_H_

#include <stdint.h>
#include <stdarg.h>

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

void print_char(char c);
void print_str(const char *str);
void scan_str(char *str);
void usart_init(void);
int strlen(const char *str);
int strcmp(const char *s1, const char *s2);
int strncmp(char *s1, char *s2, int len);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, char *src, int len);
char *memcpy(char *dest, char *src, int count);
void myprintf(const char *format, ...);
int sscanf_str(const char *input_str, const char *format, ...);
char *strcat(char *dest, const char *src);
void flush();

#endif 