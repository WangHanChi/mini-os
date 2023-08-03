#include <stdint.h>
#include "cli.h"
#include "reg.h"

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

void print_str(const char *str, int len)
{
    for(uint8_t i = 0; i < len; ++i){
        while (!(*(USART3_SR) &USART_FLAG_TXE))
            ;
        *(USART3_DR) = (*str++ & 0xFF);
		/*Waiting for the transmit complete (bit 6 TC)*/
        while (!(*USART3_SR & 0x40))
            ;
    }
}

uint8_t scan_str()
{
    uint8_t data;
    /*Waiting for the transmit data transmit to USART_RDR register (USART_ISR
     * bit 5 RXNE)*/
    while (!(*USART3_SR & 0x20))
        ;
    /*Receive data*/
    data = *USART3_DR;
    /*Clear RXNE by set 0*/
    *USART3_SR &= ~(0x1 << 5);
    return data;
}

/* greet is a global variable
 * This variable will be load by the loader at LMA and will be
 * copy to VMA by startup.c during startup.
 *
 * Add a global variable greet instead of a string literal
 * because string literal is in .rodata region and is put
 * under the .text region by the linker script
 */ 

int _write(int file, char *ptr, int len)
{
    print_str((char *) ptr, len);
    // print_str((char *) "\r", 1);
    return len;
}

int _read(int file, char *ptr, int len)
{
    for (int i = 0; i < len; i++) {
        *ptr = (char) scan_str();
        if (*ptr == '\r')
            break; /* read Enter */
        print_str((char *) ptr++, 1);
    }
    print_str((char *) "\n\r", 2);
    return len;
}

int main(void)
{
    usart_init();

    struct File_Structure foo;
    foo.cwd = (Dirname)malloc(2); // Initialize cwd to "/"
    strcpy(foo.cwd, "/");

    INIT_LIST_HEAD(&foo.dir_tree); // Initialize the list head
    struct DirNode* root_dir = (struct DirNode*)malloc(sizeof(struct DirNode));
    root_dir->dir_name = (Dirname)malloc(2);
    strcpy(root_dir->dir_name, "/");
    list_add(&root_dir->list, &foo.dir_tree); // Add the root directory node

    char line[256];
    char cmd[10];
    char arg[100];

    int count = 1;

    while (1) {
restart:
        printf("\r%d>%s-> ", count++, foo.cwd);
        fgets(line, sizeof(line), stdin);
        fflush(stdin); /*clear buffer*/
        sscanf(line, "%s", cmd);

        if (strcmp(cmd, "ls") == 0 || strcmp(cmd, "dir") == 0) {
            ls(&foo);
        } else if (strcmp(cmd, "cd") == 0) {
            sscanf(line, "%*s %s", arg);
            cd(&foo, arg);
        } else if (strcmp(cmd, "mkdir") == 0) {
            sscanf(line, "%*s %s", arg);
            mkdir(&foo, arg);
        } else if (strcmp(cmd, "rmdir") == 0) {
            sscanf(line, "%*s %s", arg);
            rmdir(&foo, arg);
        } else if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "exit") == 0) {
            printf("ShutDown !!\n\r");
            while(1){
                printf("If you want to restart, please enter 'r'\n\r");
                char check;
                scanf("%c", &check);
                if(check == 'r')
                    goto restart;
                fflush(stdin);
            }
            free(foo.cwd);
            struct list_head* iter, * next;
            struct DirNode* node;
            list_for_each_safe(iter, next, &foo.dir_tree) {
                node = list_entry(iter, struct DirNode, list);
                free(node->dir_name);
                free(node);
            }
            return 0;
        }
    }

	return 0;
}