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

# compiler flags
CFLAGS := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -O3 -Wall -fdata-sections -ffunction-sections -g -gdwarf-2 -MMD -MP
ASMFLAGS := -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -O3 -Wall -fdata-sections -ffunction-sections -g -gdwarf-2 -MMD -MP
LINKFLAGS := -T${SRC_DIRS}/stm32f429_gcc.ld \
-mcpu=cortex-m4 \
-mthumb \
-mfpu=fpv4-sp-d16 \
-mfloat-abi=hard \
-lc \
-lm \
-Wl,--gc-sections \
-Wl,-Map=$(BUILD_DIR)/${TARGET}.map,--cref

# application source files
APP_SOURCES := $(wildcard application/*.c)

C_INCLUDES := -I${SRC_DIRS}/application \
-I../00-Extend_Library/dsp_lib/Include \
-I../00-Drivers/ \
-I../00-STM32F4xx_HAL_Driver/Inc/ \
-I../00-Usr_Library/logger/

# HAL source files
HAL_SOURCES += \
../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
../00-Extend_Library/system_stm32f4xx.c \
../00-Extend_Library/syscalls.c \
../00-Extend_Library/sysmem.c

# driver source files
DRV_SOURCES += \
../00-Drivers/drv_gpio.c \
../00-Drivers/drv_exti.c \
../00-Drivers/drv_target.c \
../00-Usr_Library/logger/logger.c

# startup source files
ASM_SOURCES += \
startup_stm32f429xx.s

# all c sources
C_SOURCES := ${APP_SOURCES} ${HAL_SOURCES} ${DRV_SOURCES}

# all object files - use basename to remove directory paths
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

# compile entry
all : $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) $(C_INCLUDES) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR) 
	$(AS) -c $(ASMFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LINKFLAGS) -o $(BUILD_DIR)/$(TARGET).elf
	$(CROSS_COMPILE)objcopy -Oihex $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex
	$(CROSS_COMPILE)objcopy -O binary $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).bin
	$(CROSS_COMPILE)size $(BUILD_DIR)/${TARGET}.elf

clean:
	rm -f $(BUILD_DIR)/*