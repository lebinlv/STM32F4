#include "DACxx6x.h"

void DACxx6x_Init(DACxx6x_t *DACxx6x_s)
{
    GPIO_InitTypeDef Temp_GPIO_Structure = {
        .GPIO_Mode = GPIO_Mode_OUT,
        .GPIO_OType = GPIO_OType_PP,
        .GPIO_Speed = GPIO_Speed_100MHz,
        .GPIO_PuPd = GPIO_PuPd_UP};
    // Init CLRb Pin:
    Temp_GPIO_Structure.GPIO_Pin = DACxx6x_s->CLRb_pin.Num;
    GPIO_Init(DACxx6x_s->CLRb_pin.Group, &Temp_GPIO_Structure);
    GPIO_SetBits(DACxx6x_s->CLRb_pin.Group, DACxx6x_s->CLRb_pin.Num);
    // Init SYNCb Pin:
    Temp_GPIO_Structure.GPIO_Pin = DACxx6x_s->SYNCb_pin.Num;
    GPIO_Init(DACxx6x_s->SYNCb_pin.Group, &Temp_GPIO_Structure);
    GPIO_SetBits(DACxx6x_s->SYNCb_pin.Group, DACxx6x_s->SYNCb_pin.Num);
    // Init SCLK Pin
    Temp_GPIO_Structure.GPIO_Pin = DACxx6x_s->SCLK_pin.Num;
    GPIO_Init(DACxx6x_s->SCLK_pin.Group, &Temp_GPIO_Structure);
    GPIO_SetBits(DACxx6x_s->SCLK_pin.Group, DACxx6x_s->SCLK_pin.Num);
    //Synchronous LDAC update mode, LDAC remains low.
    Temp_GPIO_Structure.GPIO_Pin = DACxx6x_s->LDACb_pin.Num;
    GPIO_Init(DACxx6x_s->LDACb_pin.Group, &Temp_GPIO_Structure);
    GPIO_ResetBits(DACxx6x_s->LDACb_pin.Group, DACxx6x_s->LDACb_pin.Num);
    // Init DIN Pin
    Temp_GPIO_Structure.GPIO_Pin = DACxx6x_s->DIN_pin.Num;
    GPIO_Init(DACxx6x_s->DIN_pin.Group, &Temp_GPIO_Structure);
    GPIO_ResetBits(DACxx6x_s->DIN_pin.Group, DACxx6x_s->DIN_pin.Num);
}

void DACxx6x_SendData(DACxx6x_t *DACxx6x_s, u32 data)
{
    u8 i = 0;
    GPIO_ResetBits(DACxx6x_s->SYNCb_pin.Group, DACxx6x_s->SYNCb_pin.Num);

    for (i = 0; i < 24; i++)
    {
        if (data & 0x800000)
            GPIO_SetBits(DACxx6x_s->DIN_pin.Group, DACxx6x_s->DIN_pin.Num);
        else
            GPIO_ResetBits(DACxx6x_s->DIN_pin.Group, DACxx6x_s->DIN_pin.Num);

        for (int i = 0; i < 2; i++)
            ;
        GPIO_ResetBits(DACxx6x_s->SCLK_pin.Group, DACxx6x_s->SCLK_pin.Num);
        for (int i = 0; i < 2; i++)
            ;
        GPIO_SetBits(DACxx6x_s->SCLK_pin.Group, DACxx6x_s->SCLK_pin.Num);
        data <<= 1;
    }

    GPIO_SetBits(DACxx6x_s->SYNCb_pin.Group, DACxx6x_s->SYNCb_pin.Num);
}

/** @brief 打开指定的通道。
  * @param Channel: It can be DAC_A, DAC_B or ALL_Chanel;
  */
void DACxx6x_Power_Up(DACxx6x_t *DACxx6x_s, DACxx6x_CHANEL_SEL Channel)
{
    u32 temp = POWER_COMMAND | Channel;
    DACxx6x_SendData(DACxx6x_s, temp);
}

/** 
  * @brief 使用指定的方式关闭指定的通道。
  * @param Channel: It can be DAC_A, DAC_B or ALL_Chanel;
  * @param way: It can be Pull_Down_1K, Pull_Down_100K or Hi_Z;
  */
void DACxx6x_Power_Down(DACxx6x_t *DACxx6x_s, DACxx6x_CHANEL_SEL Channel, DACxx6x_POWER_DOWN_WAY way)
{
    u32 temp = POWER_COMMAND | way | Channel;
    DACxx6x_SendData(DACxx6x_s, temp);
}

/** @brief Disable/Enable LDAC Functionality.
  * @param Channel: It can be DAC_A, DAC_B, ALL_Chanel or None:
  *                 DAC_A: LDAC pin inactive for DAC-A; active for DAC-B; 
  *                 DAC_B: LDAC pin inactive for DAC-B; active for DAC-A;
  *            ALL_Chanel: LDAC pin inactive for DAC-B and DAC-A;
  *                  None: LDAC pin active for DAC-B and DAC-A;
  */
void DACxx6x_Disable_LDAC(DACxx6x_t *DACxx6x_s, DACxx6x_CHANEL_SEL Channel)
{
    u32 temp = LDAC_COMMAND | Channel;
    DACxx6x_SendData(DACxx6x_s, temp);
}

/** 
  * @brief Set DACs to GAIN = 1 or GAIN = 2.
  * @param Channel: It can be DAC_A, DAC_B, ALL_Chanel or None:
  *                 DAC_A: DAC-A gain = 1, DAC-B gain = 2; 
  *                 DAC_B: DAC-B gain = 1, DAC-A gain = 2;
  *            ALL_Chanel: DAC-B gain = 1, DAC-A gain = 1 (power-on default);
  *                  None: DAC-B gain = 2, DAC-A gain = 2 (default with internal VREF);
  */
void DACxx6x_Gain_Set_1(DACxx6x_t *DACxx6x_s, DACxx6x_CHANEL_SEL Channel)
{
    u32 temp = GAIN_SET_COMMAND | Channel;
    DACxx6x_SendData(DACxx6x_s, temp);
}

/** @brief Enable or Disable internal VREF.
  * @param New_State: It can be VREF_DISABLE or VREF_ENABLE:
  *        VREF_DISABLE: Disable internal reference and reset DACs to gain = 1;
  *         VREF_ENABLE: Enable internal reference and reset DACs to gain = 2;
  */
void DACxx6x_Internal_Vref_Set(DACxx6x_t *DACxx6x_s, DACxx6x_VREF_State New_State)
{
    u32 temp = VREF_COMMAND | New_State;
    DACxx6x_SendData(DACxx6x_s, temp);
}

void DACxx6x_DACA_OUT(DACxx6x_t *DACxx6x_s, u16 n_mV)
{
    u32 value = (u32)(DACxx6x_s->r_Resolution * n_mV);
    value |= WRITE_DACA_REG_UPDATE_A;
    DACxx6x_SendData(DACxx6x_s, value);
}

void DACxx6x_DACB_OUT(DACxx6x_t *DACxx6x_s, u16 n_mV)
{
    u32 value = (u32)(DACxx6x_s->r_Resolution * n_mV);
    value |= WRITE_DACB_REG_UPDATE_B;
    DACxx6x_SendData(DACxx6x_s, value);
}

void DACxx6x_BOTH_OUT(DACxx6x_t *DACxx6x_s, u16 n_mV)
{
    u32 value = (u32)(DACxx6x_s->r_Resolution * n_mV);
    value |= WRITE_BOTH_REG_UPDATE_ALL;
    DACxx6x_SendData(DACxx6x_s, value);
}

void DACxx6x_DACA_Bipolar_OUT(DACxx6x_t *DACxx6x_s, int16_t n_mV)
{
    int16_t temp = n_mV + DACxx6x_s->offset_mV;
    if (temp < 0)
        return;
    DACxx6x_DACA_OUT(DACxx6x_s, (u16)temp);
}
