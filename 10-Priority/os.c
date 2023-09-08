#include "os.h"
#include <stddef.h>
#include <stdint.h>
#include "malloc.h"
#include "reg.h"
#include "thread.h"

int *task_prio[6];

void busy_loop(void *str)
{
    while (1) {
        print_str(str);
        myprintf(": Running...");
        delay(9000);
    }
}

void test(void *userdata)
{
    myprintf("\n\rIn test \n");
    busy_loop(userdata);
}

int main(void)
{
    char *str[6];
    char buffer[10];

    usart_init();

    for (int i = 0; i < 6; ++i) {
        /* Set initial task priority */
        task_prio[i] = malloc(sizeof(int));
        *(task_prio[i]) = i;

        /* Set task name */
        int j = 0;
        while (1) {
            buffer[j] = "\n\rTaskX "[j];
            if (buffer[j] == '\0')
                break;
            j++;
        }
        buffer[6] = '0' + i;
        buffer[7] = ' ';

        str[i] = malloc(j + 1);
        strcpy(str[i], buffer);

        /* Create task */
        if (thread_create(test, (void *) str[i], task_prio[i]) == -1)
            myprintf("Thread %d creation failed\n\r", i);
    }
    /* SysTick configuration */
    *SYSTICK_RVR = (CPU_CLOCK_HZ / TICK_RATE_HZ) - 1UL;
    *SYSTICK_CVR = 0;
    *SYSTICK_CSR = 0x03;

    print_str("\n\rStart to schedule!\n\r");
    thread_start();

    return 0;
}