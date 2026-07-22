@REM 构建完整编译项目

echo off
setlocal enabledelayedexpansion

@REM set BuildBasicPathList[0]=01-00_STM32F429_START\project
@REM set BuildBasicPathList[1]=01-01_STM32F429_RCC\project
@REM set BuildBasicPathList[2]=01-02_STM32F429_GPIO\project
@REM set BuildBasicPathList[3]=01-03_STM32F429_EXTI\project
@REM set BuildBasicPathList[4]=01-04_STM32F429_USART\project
@REM set BuildBasicPathList[5]=01-05_STM32F429_RTC\project
@REM set BuildBasicPathList[6]=01-06_STM32F429_DAC\project
@REM set BuildBasicPathList[7]=01-07_STM32F429_ADC\project
@REM set BuildBasicPathList[8]=01-08_STM32F429_SPI\project
@REM set BuildBasicPathList[9]=01-09_STM32F429_I2C\project
@REM set BuildBasicPathList[10]=01-10_STM32F429_TIME\project
@REM set BuildBasicPathList[11]=01-11_STM32F429_WD\project
@REM set BuildBasicPathList[12]=01-12_STM32F429_DMA\project
@REM set BuildBasicPathList[13]=01-13_STM32F429_ALG\project
@REM set BuildBasicPathList[14]=01-14_STM32F429_SAI\project
@REM set BuildBasicPathList[15]=01-15_STM32F429_FLASH\project
@REM set BuildBasicPathList[16]=01-16_STM32F429_LP\project

@REM for /l %%n in (0,1,16) do (
@REM     echo start build !BuildBasicPathList[%%n]!

@REM     if exist "!BuildBasicPathList[%%n]!\stm32f429.uvprojx" (
@REM         UV4.exe -j0 -b !BuildBasicPathList[%%n]!\stm32f429.uvprojx -t stm32f429 -o Listings\uv4.log
@REM         type "!BuildBasicPathList[%%n]!\Listings\uv4.log"
@REM     ) else (
@REM         echo "!BuildBasicPathList[%%n]!\stm32f429.uvprojx" not exist
@REM         sleep 1
@REM     )
@REM )

@REM set Build2PathList[0]=02-01_STM32F429_SDIO\project
@REM set Build2PathList[1]=02-02_STM32F429_CAN\project
@REM set Build2PathList[2]=02-03_STM32F429_FMC_SDRAM\project
@REM set Build2PathList[3]=02-04-1_STM32F429_FMC\project
@REM set Build2PathList[4]=02-04-2_STM32F429_FMC_TSC\project
@REM set Build2PathList[5]=02-05_STM32F429_DCMI\project
@REM set Build2PathList[6]=02-06_STM32F429_USB_SLAVE\project
@REM set Build2PathList[7]=02-07_STM32F429_USB_HOST\project
@REM set Build2PathList[8]=02-08_STM32F429_TINY_USB\project
@REM set Build2PathList[9]=02-09_STM32F429_LWIP\project

@REM for /l %%n in (0,1,9) do (
@REM     echo start build !Build2PathList[%%n]!

@REM     if exist "!Build2PathList[%%n]!\stm32f429.uvprojx" (
@REM         UV4.exe -j0 -b !Build2PathList[%%n]!\stm32f429.uvprojx -t stm32f429 -o Listings\uv4.log
@REM         type "!Build2PathList[%%n]!\Listings\uv4.log"
@REM     ) else (
@REM         echo "!Build2PathList[%%n]!\stm32f429.uvprojx" not exist
@REM         sleep 1
@REM     )
@REM )

set Build3PathList[0]=03-01-1_STM32F429_FreeRTOS\project
set Build3PathList[1]=03-02-1_STM32F429_RTThread_Nano\project
set Build3PathList[2]=03-03-1_STM32F429_ModbusNoOS\project
set Build3PathList[3]=03-03-2_STM32F429_ModbusOS\project
set Build3PathList[4]=03-04_STM32F429_MQTT\project
set Build3PathList[5]=03-05_STM32F429_EMWIN\project
set Build3PathList[6]=03-06_STM32F429_LVGL\project
set Build3PathList[7]=03-07_STM32F429_TOUCHGFX\project
set Build3PathList[8]=03-08_STM32F429_CPP\project
set Build3PathList[9]=03-09_STM32F429_RUST\project
set Build3PathList[10]=03-10_STM32F429_MicroPython\project

for /l %%n in (0,1,10) do (
    echo start build !Build3PathList[%%n]!

    if exist "!Build3PathList[%%n]!\stm32f429.uvprojx" (
        UV4.exe -j0 -b !Build3PathList[%%n]!\stm32f429.uvprojx -t stm32f429 -o Listings\uv4.log
        type "!Build3PathList[%%n]!\Listings\uv4.log"
    ) else (
        echo "!Build3PathList[%%n]!\stm32f429.uvprojx" not exist
        sleep 1
    )
)

PAUSE