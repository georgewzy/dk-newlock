#include "stm32l1xx.h"
#include "simple_server.h"
#include "spi.h"
#include "led.h"
#include "adc.h"
int main(void)
{
    SPI1_Init();  
    LED_Init();  
    Adc_Init();    
    while(1)
    {
        simple_server();
    }

}



