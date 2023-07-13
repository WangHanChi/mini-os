# mini-os

This is about the mini operating system for STM32f429zi

> [Board](https://www.st.com/en/microcontrollers-microprocessors/stm32f429zi.html)

## Implement

- Using GPIOD alternate function for USART3
- Create user task
- Multi-tasking
- Context switch
- Interrupt from Timer
- Preemept


## TODO

- [ ] Import CMSIS
- [ ] Wrapper `malloc` with `brk()`
- [ ] Implement memory allocator
- [ ] CLI window
- [ ] Bootloader

## Reference link 

- [jserv / mini-arm-os](https://github.com/jserv/mini-arm-os)
