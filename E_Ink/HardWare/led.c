# include "led.h"

# ifdef USE_Library

void LED_On(u8 chosed_led)
{
    switch (chosed_led)
    {
        case 0:
            GPIO_ResetBits(GPIOC,GPIO_Pin_0);
        break;
        case 1:
            GPIO_ResetBits(GPIOD,GPIO_Pin_3);
        break;
    }
}
void LED_Off(u8 chosed_led)
{
    switch (chosed_led)
    {
        case 0:
            GPIO_SetBits(GPIOC,GPIO_Pin_0);
        break;
        case 1:
            GPIO_SetBits(GPIOD,GPIO_Pin_3);
        break;
    }
}
# endif


void LED_Init(void)
{
    GPIO_InitTypeDef LED_GPIO_Structure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD, ENABLE);
    LED_GPIO_Structure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_3;
    LED_GPIO_Structure.GPIO_Mode = GPIO_Mode_OUT;
    LED_GPIO_Structure.GPIO_OType = GPIO_OType_PP;
    LED_GPIO_Structure.GPIO_Speed = GPIO_Speed_100MHz;
    LED_GPIO_Structure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &LED_GPIO_Structure);
    GPIO_Init(GPIOD, &LED_GPIO_Structure);
    GPIO_SetBits(GPIOC,GPIO_Pin_0);
    GPIO_SetBits(GPIOD,GPIO_Pin_3);
}
