#ifndef LED_H_
#define LED_H_
#include "stm32f4xx.h"

class OnBoard_LED
{
  public:
    OnBoard_LED();
    ~OnBoard_LED();
    void Init(void);
    void On(u8 chosed_led);
    void Off(u8 chosed_led);
};

#endif
