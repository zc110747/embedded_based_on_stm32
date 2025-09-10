# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.16)

# 设置项目名称
project(stm32_gcc)

# 启用C语言和GNU汇编语言支持
enable_language(ASM)

# 设置交叉编译环境
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

# 设置编译选项
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -O3 -Wall -fdata-sections -ffunction-sections -g -gdwarf-2 -MMD -MP")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -O3 -Wall -fdata-sections -ffunction-sections -g -gdwarf-2 -MMD -MP")

add_executable(${PROJECT_NAME}.elf)

# 导入文件到编译工具中
# 增加本地文件
file(GLOB LOCAL_FILE 
        "application/*.c"
        "startup_stm32f429xx.s")

# 增加驱动文件
file(GLOB USER_DRIVERS 
    "../00-Drivers/drv_gpio.c"
    "../00-Drivers/drv_exti.c"
    "../00-Drivers/drv_target.c")

# 增加 HAL 库文件
file(GLOB HAL_LIB_SOURCES 
    "../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c"
    "../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c"
    "../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c"
    "../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c"
    "../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c"
    "../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c"
    "../00-STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c"
    "../00-Extend_Library/system_stm32f4xx.c"
    "../00-Extend_Library/syscalls.c")

target_sources(${PROJECT_NAME}.elf PRIVATE 
    "${LOCAL_FILE}"
    "${HAL_LIB_SOURCES}"
    "${USER_DRIVERS}")

# 添加头文件目录
target_include_directories(${PROJECT_NAME}.elf PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/application"
    "../00-Extend_Library/dsp_lib/Include"
    "../00-Drivers"
    "../00-STM32F4xx_HAL_Driver/Inc")

target_link_options(${PROJECT_NAME}.elf PRIVATE 
    "-T${CMAKE_CURRENT_SOURCE_DIR}/stm32f429_gcc.ld"
    "-mcpu=cortex-m4"
    "-mthumb"
    "-mfpu=fpv4-sp-d16"
    "-mfloat-abi=hard"
    "-specs=nano.specs"
    "-lc"
    "-lm"
    "-Wl,--gc-sections"
    "-Wl,-Map=${PROJECT_NAME}.map,--cref")

# 设置编译定义
target_compile_definitions(${PROJECT_NAME}.elf PRIVATE USE_HAL_DRIVER STM32F429xx)

# 定义编译后选项
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
)