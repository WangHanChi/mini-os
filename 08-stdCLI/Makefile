CROSS_COMPILE ?= arm-none-eabi-
CC := $(CROSS_COMPILE)gcc
CFLAGS = -fno-common -ffreestanding -O0 -std=gnu99 \
	 -gdwarf-2 -g3 -Wall -Werror \
	 -mcpu=cortex-m4 -mthumb \
	 -Wl,-Tos.ld -nostartfiles \
	 --specs=nosys.specs \

TARGET= os
all: clean $(TARGET)

$(TARGET): os.c startup.c cli.c syscalls.c
	$(CC) $(CFLAGS) $^ -o $(TARGET)
	$(CROSS_COMPILE)objdump -S $(TARGET) > $(TARGET).list
	@echo "\n===================================================="
	$(CROSS_COMPILE)size $(TARGET)
	@echo "====================================================\n"

upload: all
	openocd -f interface/stlink.cfg -f board/st_nucleo_f4.cfg -c " program $(TARGET) exit "

qemu: $(TARGET)
	@qemu-system-arm -M ? | grep stm32-p103 >/dev/null || exit
	@echo "Press Ctrl-A and then X to exit QEMU"
	@echo
	qemu-system-arm -M stm32-p103 -nographic -kernel hello.bin

debug:
	openocd -f board/st_nucleo_f4.cfg

clean:
	rm -f *.o *.bin *.elf *.list os