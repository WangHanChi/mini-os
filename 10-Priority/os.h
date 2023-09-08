#ifndef __OS_H_
#define __OS_H_

#include <stdarg.h>
#include "reg.h"

#define MAX_TASKS 10
#define STACK_SIZE 256
#define MAX_HEAPS 40960
#define MAX_PRIORITY 8

/* 180 MHz */
#define CPU_CLOCK_HZ 180000000

/* 50 ms per tick. */
#define TICK_RATE_HZ 20

void print_char(char c);
void print_str(const char *str);
void myprintf(const char *format, ...);
void delay(volatile int count);
void strcpy(char *dest, const char *src);
void usart_init(void);

#endif