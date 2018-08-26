# include "led.h"

OnBoard_LED::OnBoard_LED(){};
OnBoard_LED::~OnBoard_LED(){};

void OnBoard_LED::On(u8 chosed_led)
{
    switch (chosed_led)
    {
    case 0:
        GPIO_ResetBits(GPIOC, GPIO_Pin_0);
        break;
    case 1:
        GPIO_ResetBits(GPIOD, GPIO_Pin_3);
        break;
    }
}
void OnBoard_LED::Off(u8 chosed_led)
{
    switch (chosed_led)
    {
    case 0:
        GPIO_SetBits(GPIOC, GPIO_Pin_0);
        break;
    case 1:
        GPIO_SetBits(GPIOD, GPIO_Pin_3);
        break;
    }
}

void OnBoard_LED::Init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD, ENABLE);
    
    GPIO_InitTypeDef LED_GPIO_Structure;
    LED_GPIO_Structure.GPIO_Mode = GPIO_Mode_OUT;
    LED_GPIO_Structure.GPIO_OType = GPIO_OType_PP;
    LED_GPIO_Structure.GPIO_Speed = GPIO_Speed_100MHz;
    LED_GPIO_Structure.GPIO_PuPd = GPIO_PuPd_UP;
    
    LED_GPIO_Structure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOC, &LED_GPIO_Structure);
    GPIO_SetBits(GPIOC, GPIO_Pin_0);
    
    LED_GPIO_Structure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOD, &LED_GPIO_Structure);
    GPIO_SetBits(GPIOD,GPIO_Pin_3);
}
