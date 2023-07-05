# Generic Makefile
OUT ?= build
BIN ?= main

# c source file
INPUT_C_SRC ?= Src/main.c
C_SRC = Src/syscalls.c \
	Src/myusart.c \
	$(INPUT_C_SRC)

INPUT_C_INC ?=
C_INC = -IInc \
	$(INPUT_C_INC)

# assembly file
ASM_SRC = Src/startup.s

# linker script
LDSCRIPT = Src/link.ld

VPATH  = $(dir $(C_SRC))
VPATH += $(dir $(ASM_SRC))

# object file
OBJS  = $(patsubst %.c, $(OUT)/%.o, $(notdir $(C_SRC)))
OBJS += $(patsubst %.s, $(OUT)/%.o, $(notdir $(ASM_SRC)))

# toolchain
TOOLCHAIN = arm-none-eabi-
CC = $(TOOLCHAIN)gcc
SZ = $(TOOLCHAIN)size

# Serial port setting
PORTNAME = /dev/ttyACM0
BAUDRATE = 38400

# toolchain options
MCU = -mcpu=cortex-m4
CFLAGS = $(MCU) -mthumb $(C_INC) -O0 -Wall
LDFLAGS = $(MCU) -specs=nano.specs -T $(LDSCRIPT) -lc -lm -lnosys

all: $(OUT)/$(BIN)
# compile
$(OUT)/%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(OUT)/%.o: %.s
	$(CC) -c $(CFLAGS) $< -o $@

# link
$(OUT)/$(BIN): $(OUT) $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@
	$(SZ) $@

$(OUT):
	mkdir $@

.PHONY: disassembly load upload clean
disassembly: $(OUT)/$(BIN)
	$(TOOLCHAIN)objdump -d $^ > $(OUT)/$(BIN).S

debug:
	openocd -f board/st_nucleo_f4.cfg

upload:
	openocd -f interface/stlink.cfg -f board/st_nucleo_f4.cfg -c " program $(OUT)/$(BIN) exit "

putty:
	python3 putty.py $(PORTNAME) $(BAUDRATE)

clean:
	-@rm -r $(OUT)