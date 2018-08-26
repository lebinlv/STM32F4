#include "delay.h"
#include "OLED.h"
#include "led.h"
#include "usart.h"
#include "Dialog_plain_12.h"
#include "images.h"
int main(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
    delay_init(168);
    uart_init(115200);
    
    OnBoard_LED LED;
    LED.Init();
    
    I2C_OLED oled;
    oled.Init(GPIOA, GPIO_Pin_11,GPIOA, GPIO_Pin_12);
    oled.setFont(Dialog_plain_12);
    char temp1[] = "Welcome to ESP32!!!!\n    \"Just Do it\"\n ??????......!!!!!!";

    
    oled.drawString(10,0,temp1);
    delay_ms(2000);
    oled.clear();
    oled.drawFastImage(32,0,UESTC_Logo_width,UESTC_Logo_height,UESTC_Logo);
    delay_ms(2000);
    oled.clear();
   
   
    oled.drawLine(1,1,126,62);
    oled.drawLine(126,1,1,62);  
    oled.drawHorizontalLine(1,1,126);
    oled.drawHorizontalLine(1,62,126);
    oled.drawVerticalLine(1,1,62);
    oled.drawVerticalLine(126,1,62);
    
    oled.clear();
    
    for(int i=0;i<61;i+=4)
        oled.drawRect(i,i/2,128-2*i,64-i);
    
    oled.drawFilledRect(0,0,128,64,oled.CLEAR);   
    
    
    oled.drawFilledCircle(0,0,64);
    oled.drawFilledCircle(127,63,64);
    
    oled.drawFilledCircle(63,31,31,oled.CLEAR);
    oled.drawCircle(63,31,29);
    oled.drawFilledCircle(63,31,25,oled.NORMAL);
    oled.drawFilledCircle(63,31,22,oled.CLEAR);
    oled.drawCircle(63,31,20);
    oled.drawFilledCircle(63,31,15);

    oled.drawFilledRect(0,0,128,64, oled.INVERSE);
    oled.clear();

    int j=0;
    for(int y=0;y<8;y++)
        for(int i=0;i<128;i++)
            {
               j=j&7;
               oled.drawVerticalLine(i,j+8*y,8-j,1,oled.INVERSE);
               j++;
            }
    delay_ms(2000);
    oled.clear();
    oled.drawFastImage(0,0,WiFi_Logo_width,WiFi_Logo_height,WiFi_Logo);
        


    while(1)
    {
        LED.On(0);
        LED.Off(1);
        printf("LED0: On; LED1:Off\n");
        delay_ms(2000);
        LED.On(1);
        LED.Off(0);
        printf("LED0: Off; LED1:On\n");
        delay_ms(2000);
        
    }
}


