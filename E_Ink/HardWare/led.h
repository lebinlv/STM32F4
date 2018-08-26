#ifndef __LED_H_
#define __LED_H_
#include "stm32f4xx.h"

#define USE_Library
//# define USE_BIT


extern void LED_Init(void);

#ifdef USE_Library
extern void LED_On(u8 chosed_led);
extern void LED_Off(u8 chosed_led);
#endif


# ifdef USE_BIT
# include "sys.h"
# define LED_On(0)    (PCout(0)=1)
# define LED_Off(0)   (PCout(0)=0)
# define LED_On(1)    (PDout(3)=1)
# define LED_Off(1)   (PDout(3)=0)
# endif

# endif
