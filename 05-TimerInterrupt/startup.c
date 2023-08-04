#include <stdint.h>
#include "reg.h"

/* Bit definition for RCC_CR register */
#define RCC_CR_HSION        ((uint32_t) 0x00000001) /*!< Internal High Speed clock enable */
#define RCC_CR_HSEON        ((uint32_t) 0x00010000) /*!< External High Speed clock enable */
#define RCC_CR_HSERDY       ((uint32_t) 0x00020000) /*!< External High Speed clock ready flag */
#define RCC_CR_CSSON        ((uint32_t) 0x00080000) /*!< Clock Security System enable */

/* Bit definition for RCC_PLLCFGR register */
#define RCC_PLLCFGR_PLLSRC  ((uint32_t) 0x00400000) /*!< Internal High Speed clock enable */

/* Bit definition for RCC_CFGR register */
#define RCC_CFGR_SW         ((uint32_t) 0x00000003) /*!< SW[1:0] bits (System clock Switch) */
#define RCC_CFGR_SW_HSE     ((uint32_t) 0x00000001) /*!< HSE selected as system clock */
#define RCC_CFGR_SWS        ((uint32_t) 0x0000000C) /*!< SWS[1:0] bits (System Clock Switch Status) */
#define RCC_CFGR_HPRE_DIV1  ((uint32_t) 0x00000000)  /*!< SYSCLK not divided */
#define RCC_CFGR_PPRE1_DIV1 ((uint32_t) 0x00000000) /*!< HCLK not divided */
#define RCC_CFGR_PPRE2_DIV1 ((uint32_t) 0x00000000) /*!< HCLK not divided */

/* Bit definition for FLASH_ACR register */
#define FLASH_ACR_LATENCY   ((uint8_t) 0x7)    /*!< LATENCY[2:0] bits (Latency) */
#define FLASH_ACR_LATENCY_0 ((uint8_t) 0x00) /*!< Bit 0 */
#define FLASH_ACR_PRFTBE    ((uint8_t) 0x100)   /*!< Prefetch Buffer Enable */

#define HSE_STARTUP_TIMEOUT ((uint16_t) 0x0500) /*!< Time out for HSE start up */

#define PLL_M      8
#define PLL_N      360
#define PLL_P      2
#define PLL_Q      7

uint32_t DEFAULT_F_CLK;
uint32_t BAUDRATE;

/* main program entry point */
extern void main(void);

/* start address for the initialization values of the .data section.
defined in linker script */
extern uint32_t _sidata;
/* start address for the .data section. defined in linker script */
extern uint32_t _sdata;
/* end address for the .data section. defined in linker script */
extern uint32_t _edata;
/* start address for the .bss section. defined in linker script */
extern uint32_t _sbss;
/* end address for the .bss section. defined in linker script */
extern uint32_t _ebss;
/* end address for the stack. defined in linker script */
extern uint32_t _estack;

void rcc_clock_init(void);
void PLL_Init(void);

void Reset_Handler(void)
{
    /* Copy the data segment initializers from flash to SRAM */
    uint32_t *idata_begin   = &_sidata;
    uint32_t *data_begin    = &_sdata;
    uint32_t *data_end      = &_edata;
    while (data_begin < data_end)
        *data_begin++ = *idata_begin++;

    /* Zero fill the bss segment. */
    uint32_t *bss_begin     = &_sbss;
    uint32_t *bss_end       = &_ebss;
    while (bss_begin < bss_end)
        *bss_begin++ = 0;

    /* Clock system initialization */
    //rcc_clock_init();
    /* Use PLL (180 MHz) System clock */
    PLL_Init();

    main();
}

void default_handler(void)
{
	while (1);
}

void nmi_handler(void) __attribute((weak, alias("default_handler")));
void hardfault_handler(void) __attribute((weak, alias("default_handler")));
void memmanage_handler(void) __attribute((weak, alias("default_handler")));
void busfault_handler(void) __attribute((weak, alias("default_handler")));
void usagefault_handler(void) __attribute((weak, alias("default_handler")));
void svc_handler(void) __attribute((weak, alias("default_handler")));
void pendsv_handler(void) __attribute((weak, alias("default_handler")));
void systick_handler(void) __attribute((weak, alias("default_handler")));

__attribute((section(".isr_vector")))
uint32_t *isr_vectors[] = {
	(uint32_t *) &_estack,			/* stack pointer */
	(uint32_t *) Reset_Handler,		/* code entry point */
	(uint32_t *) nmi_handler,		/* NMI handler */
	(uint32_t *) hardfault_handler,		/* hard fault handler */
	(uint32_t *) memmanage_handler,		/* mem manage handler */
	(uint32_t *) busfault_handler,		/* bus fault handler */
	(uint32_t *) usagefault_handler,	/* usage fault handler */
	0,
	0,
	0,
	0,
	(uint32_t *) svc_handler,		/* svc handler */
    0,
	0,
	(uint32_t *) pendsv_handler,		/* pendsv handler */
	(uint32_t *) systick_handler		/* systick handler */
};

void rcc_clock_init(void)
{
    /* Reset the RCC clock configuration to the default reset state(for debug
     * purpose) */
    /* Set HSION bit */
    *RCC_CR         |= (uint32_t) 0x00000001;

    /* Reset HSEON, CSSON and PLLON bits */
    *RCC_CR         &= (uint32_t) 0xFEF6FFFF;

    /* Reset HSEBYP bit */
    *RCC_CR         &= (uint32_t) 0xFFFBFFFF;

    /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
    *RCC_PLLCFGR    &= (uint32_t) 0x24003010;

    /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
    *RCC_CFGR       &= (uint32_t) 0x00000000;

    /* Disable all interrupts and clear pending bits  */
    *RCC_CIR        = (uint32_t) 0x00000000;

    /* Configure the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers */
    /* Configure the Flash Latency cycles and enable prefetch buffer */
    volatile uint32_t StartUpCounter = 0, HSEStatus = 0;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
    /* Enable HSE */
    *RCC_CR |= (uint32_t) RCC_CR_HSEON;

    /* Wait till HSE is ready and if Time out is reached exit */
    do {
        HSEStatus = *RCC_CR & RCC_CR_HSERDY;
        StartUpCounter++;
    } while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((*RCC_CR & RCC_CR_HSERDY) != 0)
        HSEStatus = (uint32_t) 0x01;
    else
        HSEStatus = (uint32_t) 0x00;

    if (HSEStatus == (uint32_t) 0x01) {
        /* Enable Prefetch Buffer */
        *FLASH_ACR |= FLASH_ACR_PRFTBE;

        /* Flash 0 wait state */
        *FLASH_ACR &= (uint32_t) ((uint32_t) ~FLASH_ACR_LATENCY);

        *FLASH_ACR |= (uint32_t) FLASH_ACR_LATENCY_0;

        /* HCLK = SYSCLK */
        *RCC_CFGR |= (uint32_t) RCC_CFGR_HPRE_DIV1;

        /* PCLK2 = HCLK */
        *RCC_CFGR |= (uint32_t) RCC_CFGR_PPRE2_DIV1;

        /* PCLK1 = HCLK */
        *RCC_CFGR |= (uint32_t) RCC_CFGR_PPRE1_DIV1;

        /* Set System Clock 8Mhz, and APB1 clock is divides 1 */
        DEFAULT_F_CLK = 8000000 / 1;
        BAUDRATE = 38400U;

        /* Select HSE as system clock source */
        *RCC_CFGR &= (uint32_t) ((uint32_t) ~(RCC_CFGR_SW));
        *RCC_CFGR |= (uint32_t) RCC_CFGR_SW_HSE;

        /* Wait till HSE is used as system clock source */
        while ((*RCC_CFGR & (uint32_t) RCC_CFGR_SWS) != (uint32_t) 0x04)
            ;
    } else {
        /* If HSE fails to start-up, the application will have wrong clock
        configuration. User can add here some code to deal with this error */
    }
}

void PLL_Init()
{
    /* Systrm Init */

    /* HSE ON */
    *RCC_CR |= (uint32_t) RCC_CR_HSEON;

    while (!((*RCC_CR & 0x20000) >> 17)) 
            ;

    /* SET PLL OFF  */
    *RCC_CR &= ~(1 << 24);

    /* Reset PLLCFGR register */
    *RCC_CFGR = 0x00000000;

    /* Reset PLLCFGR register */
    *RCC_PLLCFGR = 0x24003010;

    /* Reset HESON, CSSON and PLLON bits */
    //*RCC_CR &= (uint32_t) 0xFEF6FFFF;

    /************* SetSysClock ************/

    *RCC_PLLCFGR  = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16)| (PLL_Q << 24);

    /* PLL clock Source HSI or HSE */
    *RCC_PLLCFGR |= (1 << 22);

    /* APB1 PWR Enable */
    *RCC_APB1ENR |= (1 << 28);

    /* Select regulator voltage output Scale 1 mode, System frequency up to 180 MHz */
    *PWR_CR |= (1 << 14);

    /* AHB div 1 */
    *RCC_CFGR &= ~(0b1111 << 4);

    /* APB2 Div  =  2*/
    *RCC_CFGR &= ~(0b111 << 13);
    *RCC_CFGR |=  (0b100 << 13);

    /* Set System clock 180Mhz, and APB1 Max Freq is 45Mhz */
    DEFAULT_F_CLK = (180000000 / 8);
    BAUDRATE = 38400U;

    /* APB 1 Div = 8  */
    *RCC_CFGR &= ~(0b111 << 10);
    *RCC_CFGR |=  (0b110 << 10);

    /* SET PLL ON  */
    *RCC_CR |= (1 << 24);

    /* Check PLL is ready */
    while(!((*RCC_CR) & (1 << 25)))
        ;

    /* Enable the Over-drive to extend the clock frequency to 180 Mhz */
    *PWR_CR |= (1 << 16);
    while(!((*PWR_CSR) & (1 << 16)))
        ;

    *PWR_CR |= (1 << 17);
    while(!((*PWR_CSR) & (1 << 17)))
        ;
    
    *FLASH_ACR |= (1 << 8);
    *FLASH_ACR |= (1 << 9);
    *FLASH_ACR |= (1 << 10);
    *FLASH_ACR |= 0x05;

    /* Select the main PLL as system clock source */
    *RCC_CFGR &= ~(3);
    *RCC_CFGR |= (0b10);

    /* Wait till the main PLL is used as system clock source */
    while (((*RCC_CFGR & 0xc) >> 2) != 2)
        ; 
}
