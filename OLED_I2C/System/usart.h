#ifndef __USART_H
#define __USART_H
#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdio.h>
    
void uart_init(u32 bound);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
