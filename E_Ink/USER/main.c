#include <stdlib.h>
//#include "usart.h"
#include "delay.h"
#include "E_LINK.h"
#include "E_LINK_Display.h"
#include "IMAGE.h"
#include "led.h"

int main()
{
    delay_init(168);
    LED_Init();

    /**  硬件连接关系
      *  E_INK_SDI     <----->     STM32_PB5
      *  E_INK_CLK     <----->     STM32_PB3
      *  E_INK_DC      <----->     STM32_PA1
      *  E_INK_CS      <----->     STM32_PA2
      */

    /**** 相关时钟使能 ****/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    
    /**** 初始化 PB5, PB3引脚为复用功能 ****/
    GPIO_InitTypeDef GPIO_Init_Structure =
    {
        .GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_3,
        .GPIO_Mode = GPIO_Mode_AF,
        .GPIO_OType = GPIO_OType_PP,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_PuPd = GPIO_PuPd_UP
    };
    GPIO_Init(GPIOB, &GPIO_Init_Structure);

    
    /**** 配置SPI复用引脚映射 ****/
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);


    /**** 设置E_INK实例 ****/
    E_INK_TypeDef E_INK_Instance = {
        .dc_pin = {GPIOA, GPIO_Pin_1},
        .cs_pin = {GPIOA, GPIO_Pin_2},
        .E_INK_SPI = SPI1,
    };

    unsigned char *frame_buffer = (unsigned char *)malloc(EPD_WIDTH * EPD_HEIGHT / 8);


    //初始化 E_INK实例，包括初始化SPI，初始化DC、CS引脚，初始化屏幕
    E_INK_Init(&E_INK_Instance, lut_full_update);  

    Paint paint;
    Paint_Init(&paint, frame_buffer, E_INK_Instance.width, E_INK_Instance.height);
    Paint_Clear(&paint, UNCOLORED);

//    /********************* 将显示内容写入缓存区 *******************/
//    Paint_DrawFilledRectangle(&paint, 20, 20, 180, 55, COLORED);
//    Paint_DrawStringAt(&paint, 35, 30, "E-INK-1.54", &Font20, UNCOLORED);
//    Paint_DrawStringAt(&paint, 35, 100, "Hello Liber!!!", &Font16, COLORED);
//    Paint_DrawFilledCircle(&paint, 100, 150, 30, COLORED);

//    /********************* 显示缓存区内的内容 *********************/
//    E_INK_SetFrameMemory(&E_INK_Instance, frame_buffer, 0, 0, Paint_GetWidth(&paint), Paint_GetHeight(&paint));
//    E_INK_DisplayFrame(&E_INK_Instance);

//    delay_ms(5000);

    E_INK_Init(&E_INK_Instance, lut_partial_update);

    /************************ 显示图片*************************/
    E_INK_SetFrameMemory(&E_INK_Instance, Image_me, 0, 0, E_INK_Instance.width, E_INK_Instance.height);
    E_INK_DisplayFrame(&E_INK_Instance);
//    E_INK_SetFrameMemory(&E_INK_Instance, Image_me, 0, 0, E_INK_Instance.width, E_INK_Instance.height);
//    E_INK_DisplayFrame(&E_INK_Instance);
//    delay_ms(2000);

//    E_INK_SetFrameMemory(&E_INK_Instance, Image_me, 0, 0, E_INK_Instance.width, E_INK_Instance.height);
//    E_INK_DisplayFrame(&E_INK_Instance);
//    E_INK_SetFrameMemory(&E_INK_Instance, Image_me, 0, 0, E_INK_Instance.width, E_INK_Instance.height);
//    E_INK_DisplayFrame(&E_INK_Instance);

    LED_On(1);
    LED_On(0);
    // time_start_ms = HAL_GetTick();
    //time_start_ms = 0;

    // /******************* 局部刷新（计时效果) *******************/
    while (1)
    {
//        time_now_s = (i - time_start_ms) * 1000;
//        time_string[0] = time_now_s / 60 / 10 + '0';
//        time_string[1] = time_now_s / 60 % 10 + '0';
//        time_string[3] = time_now_s % 60 / 10 + '0';
//        time_string[4] = time_now_s % 60 % 10 + '0';

//        Paint_SetWidth(&paint, 32); //设置区域大小
//        Paint_SetHeight(&paint, 96);
//        Paint_SetRotate(&paint, ROTATE_270); //旋转区域的方向

//        Paint_Clear(&paint, UNCOLORED);
//        Paint_DrawStringAt(&paint, 0, 4, time_string, &Font24, COLORED);                                                //写入
//        E_INK_SetFrameMemory(&E_INK_Instance, frame_buffer, 48, 45, Paint_GetWidth(&paint), Paint_GetHeight(&paint)); //显示
//        E_INK_DisplayFrame(&E_INK_Instance);

//        //E_INK_DelayMs(&E_INK_Instance, 500);
//        delay_ms(2000);
//        LED_Off(i % 2);
//        LED_On(1 - i % 2);
//        i++;
    }
}
