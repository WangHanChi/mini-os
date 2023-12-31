# mini-os

This is about the mini operating system for STM32f429zi
> [Note](https://hackmd.io/@wanghanchi/mini-os)  
> [Device](https://www.st.com/en/microcontrollers-microprocessors/stm32f429zi.html)

## Implement

- Using GPIOD alternate function for USART3
- Create user task
- Multi-tasking
- Context switch
- Interrupt from Timer
- Preemept
- Wrapper `malloc` with `brk()`
- Implement memory allocator
- CLI window (include `scanf`)

## TODO

- [ ] Import CMSIS
- [ ] Task scheduler
- [ ] Bootloader

## Reference link 

- [jserv / mini-arm-os](https://github.com/jserv/mini-arm-os)
