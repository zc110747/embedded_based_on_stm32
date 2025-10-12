
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
set BuildPathList[14]=15-STM32F429_CAN\project
set BuildPathList[15]=16-STM32F429_ALG\project
set BuildPathList[16]=17-STM32F429_FreeRTOS\project
set BuildPathList[17]=18-STM32F429_rtthread\project
set BuildPathList[18]=19-STM32F429_LWIP\project
set BuildPathList[19]=20-STM32F429_LowerPower\project

set BuildPathList[20]=22-1-STM32F429_FMC\project
set BuildPathList[21]=22-2-STM32F429_FMC_TSC\project
set BuildPathList[22]=23-1-STM32F429_ModbusNoOS\project
set BuildPathList[23]=23-2-STM32F429_ModbusOS\project
set BuildPathList[24]=24-STM32F429_DCMI\project
set BuildPathList[25]=25-STM32F429_USB_SLAVE\project

for /l %%n in (0,1,24) do (
    echo start build !BuildPathList[%%n]!
    UV4.exe -j0 -b !BuildPathList[%%n]!\stm32f429.uvprojx -t stm32f429 -o Listings\uv4.log
    type "!BuildPathList[%%n]!\Listings\uv4.log"
)

PAUSE