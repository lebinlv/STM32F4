#include "DACxx6x.h"
#include "delay.h"

DACxx6x_t DACxx6x_s = {
    .SCLK_pin = {GPIOG, GPIO_Pin_2},
    .DIN_pin = {GPIOG, GPIO_Pin_4},
    .SYNCb_pin = {GPIOG, GPIO_Pin_3},
    .CLRb_pin = {GPIOG, GPIO_Pin_5},
    .LDACb_pin = {GPIOG, GPIO_Pin_6},
    .r_Resolution = 26.214f,
};

int main()
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
    delay_init(168);

    DACxx6x_Init(&DACxx6x_s);

    DACxx6x_Disable_LDAC(&DACxx6x_s, ALL_Chanel);

    DACxx6x_Internal_Vref_Set(&DACxx6x_s, VREF_DISABLE);

    DACxx6x_Power_Up(&DACxx6x_s, ALL_Chanel);
    
    delay_ms(10);

    DACxx6x_BOTH_OUT(&DACxx6x_s, 2);

    DACxx6x_Power_Down(&DACxx6x_s, ALL_Chanel, Pull_Down_1K);

    while(1){}
}
