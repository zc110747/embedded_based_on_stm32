
echo off
setlocal enabledelayedexpansion

set BuildPathList[0]=01-STM32F429_START\project
set BuildPathList[1]=02-STM32F429_RCC\project
set BuildPathList[2]=03-STM32F429_GPIO\project
set BuildPathList[3]=04-STM32F429_EXTI\project
set BuildPathList[4]=05-STM32F429_USART\project
set BuildPathList[5]=06-STM32F429_RTC\project
set BuildPathList[6]=07-STM32F429_DAC\project
set BuildPathList[7]=08-STM32F429_ADC\project
set BuildPathList[8]=09-STM32F429_SPI\project
set BuildPathList[9]=10-STM32F429_I2C\project
set BuildPathList[10]=11-STM32F429_TIME\project
set BuildPathList[11]=12-STM32F429_WD\project
set BuildPathList[12]=13-STM32F429_DMA\project
set BuildPathList[13]=14-STM32F429_SDIO\project
set BuildPathList[14]=15-STM32F429_FMC\project
set BuildPathList[15]=16-STM32F429_CAN\project
set BuildPathList[16]=17-STM32F429_USB\project

for /l %%n in (0,1,16) do (
    echo start build !BuildPathList[%%n]!
    UV4.exe -j0 -b !BuildPathList[%%n]!\stm32f429.uvprojx -t stm32f429 -o Listings\uv4.log
    type "!BuildPathList[%%n]!\Listings\uv4.log"
)

PAUSE