#ifndef __DACXX6X_H
#define __DACXX6X_H

#include "stm32f4xx.h"

#define VREF_COMMAND                   0x380000
#define LDAC_COMMAND                   0x300000
#define RESET_INPUT_REG_UPDATE_ALL     0x280000
#define RESET_ALL_REG_UPDATE_ALL       0x280001
#define POWER_COMMAND                  0x200000
#define GAIN_SET_COMMAND               0x020000
#define UPDATE_DAC_A                   0x080000
#define UPDATE_DAC_B                   0x090000
#define UPDATE_BOTH                    0x0F0000

#define WRITE_DACA_REG                 0x000000
#define WRITE_DACB_REG                 0x010000
#define WRITE_BOTH_REG                 0x070000
#define WRITE_DACA_REG_UPDATE_A        0x180000
#define WRITE_DACB_REG_UPDATE_B        0x190000
#define WRITE_DACA_REG_UPDATE_ALL      0x100000
#define WRITE_DACB_REG_UPDATE_ALL      0x110000
#define WRITE_BOTH_REG_UPDATE_ALL      0x170000

typedef enum {DAC_A=0x01, DAC_B=0x02, ALL_Chanel=0x03, None = 0x00} DACxx6x_CHANEL_SEL;
typedef enum {Pull_Down_1K=0x10, Pull_Down_100K=0x20, Hi_Z=0x30} DACxx6x_POWER_DOWN_WAY;
typedef enum {VREF_DISABLE = 0x00, VREF_ENABLE = 0x01} DACxx6x_VREF_State;

typedef struct
{
    GPIO_TypeDef *Group;
    u16 Num;
} Pin_TypeDef;

typedef struct
{
    Pin_TypeDef SCLK_pin;
    Pin_TypeDef DIN_pin;
    Pin_TypeDef SYNCb_pin;
    Pin_TypeDef CLRb_pin;
    Pin_TypeDef LDACb_pin;
    float r_Resolution; //分辨率的倒数，r_Resolution = (2^n-1)/参考电压(mV)
    u16 offset_mV;      //偏移量，用于双极性输出，单位：mV
} DACxx6x_t;

// Public functions
void DACxx6x_Init(DACxx6x_t *DACxx6x_s);
void DACxx6x_DACA_OUT(DACxx6x_t *DACxx6x_s, u16 n_mV);
void DACxx6x_DACB_OUT(DACxx6x_t *DACxx6x_s, u16 n_mV);
void DACxx6x_BOTH_OUT(DACxx6x_t *DACxx6x_s, u16 n_mV);
void DACxx6x_DACA_Bipolar_OUT(DACxx6x_t *DACxx6x_s, int16_t n_mV);

/** @brief 打开指定的通道。
  * @param Channel: It can be DAC_A, DAC_B or ALL_Chanel;
  */
void DACxx6x_Power_Up(DACxx6x_t *DACxx6x_s, DACxx6x_CHANEL_SEL Channel);


/** @brief 使用指定的方式关闭指定的通道。
  * @param Channel: It can be DAC_A, DAC_B or ALL_Chanel;
  * @param way: It can be Pull_Down_1K, Pull_Down_100K or Hi_Z;
  */
void DACxx6x_Power_Down(DACxx6x_t *DACxx6x_s, DACxx6x_CHANEL_SEL Channel, DACxx6x_POWER_DOWN_WAY way);


/** @brief Disable/Enable LDAC Functionality.
  * @param Channel: It can be DAC_A, DAC_B, ALL_Chanel or None:
  *                 DAC_A: LDAC pin inactive for DAC-A; active for DAC-B; 
  *                 DAC_B: LDAC pin inactive for DAC-B; active for DAC-A;
  *            ALL_Chanel: LDAC pin inactive for DAC-B and DAC-A;
  *                  None: LDAC pin active for DAC-B and DAC-A;
  */
void DACxx6x_Disable_LDAC(DACxx6x_t *DACxx6x_s, DACxx6x_CHANEL_SEL Channel);


/** @brief Set DACs to GAIN = 1 or GAIN = 2.
  * @param Channel: It can be DAC_A, DAC_B, ALL_Chanel or None:
  *                 DAC_A: DAC-A gain = 1, DAC-B gain = 2; 
  *                 DAC_B: DAC-B gain = 1, DAC-A gain = 2;
  *            ALL_Chanel: DAC-B gain = 1, DAC-A gain = 1 (power-on default);
  *                  None: DAC-B gain = 2, DAC-A gain = 2 (default with internal VREF);
  */
void DACxx6x_Gain_Set_1(DACxx6x_t *DACxx6x_s, DACxx6x_CHANEL_SEL Channel);


/** @brief Enable or Disable internal VREF.
  * @param New_State: It can be VREF_DISABLE or VREF_ENABLE
  */
void DACxx6x_Internal_Vref_Set(DACxx6x_t *DACxx6x_s, DACxx6x_VREF_State New_State);



// Private functions
static void DACxx6x_SendData(DACxx6x_t *DACxx6x_s, u32 data);

#endif
