# current directory
SRC_DIRS := $(shell pwd)
BUILD_DIR = build

# compiler tools
CROSS_COMPILE := arm-none-eabi-
CC := $(CROSS_COMPILE)gcc
AS := $(CROSS_COMPILE)gcc
CCPLUS := $(CROSS_COMPILE)g++

## program information
TARGET := stm32f429_gcc

# compile flags
CFLAGS := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -O3 -Wall -fdata-sections -ffunction-sections -g -gdwarf-2 -MMD -MP
ASMFLAGS := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -O3 -Wall -fdata-sections -ffunction-sections -g -gdwarf-2 -MMD -MP
LINKFLAGS := -T${SRC_DIRS}/stm32f429_gcc.ld \
-mcpu=cortex-m4 \
-mthumb \
-mfpu=fpv4-sp-d16 \
-mfloat-abi=hard \
-specs=nano.specs \
-lc \
-lm \
-Wl,--gc-sections \
-Wl,-Map=${TARGET}.map,--cref

# application source files
APP_SOURCES := $(wildcard application/*.c)

C_INCLUDES := -I${SRC_DIRS}/application \
-I../00-Extend_Library/dsp_lib/Include \
-I../00-Drivers/ \
-I../00-STM32F4xx_HAL_Driver/Inc/

# HAL source files
HAL_SOURCES += \
../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
../00-Extend_Library/system_stm32f4xx.c \
../00-Extend_Library/syscalls.c

# driver source files
DRV_SOURCES += \
../00-Drivers/drv_gpio.c \
../00-Drivers/drv_exti.c \
../00-Drivers/drv_target.c

# startup source files
ASM_SOURCES += \
startup_stm32f429xx.s

# all c sources
C_SOURCES := ${APP_SOURCES} ${HAL_SOURCES} ${DRV_SOURCES}

# all object files
OBJECTS = $(C_SOURCES:.c=.o)
OBJECTS += $(ASM_SOURCES:.s=.o)

$(info OBJECTS = $(OBJECTS))

# compile entry
all : $(TARGET)

%.o: %.c
	$(CC) -c $(CFLAGS) $(C_INCLUDES) $< -o $@

%.o: %.s
	$(AS) -c $(ASMFLAGS) $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LINKFLAGS) -o $(TARGET).elf
	$(CROSS_COMPILE)objcopy -Oihex $(TARGET).elf $(TARGET).hex
	$(CROSS_COMPILE)objcopy -O binary $(TARGET).elf $(TARGET).bin
	$(CROSS_COMPILE)size ${TARGET}.elf

clean:
	rm -f $(OBJECTS) $(TARGET) $(TARGET).map