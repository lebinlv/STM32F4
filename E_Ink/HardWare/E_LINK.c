#include <stdlib.h>
#include "E_LINK.h"

/* private functions */
void E_INK_SetLut(E_INK_TypeDef *E_INK, const unsigned char *lut);
void E_INK_SetMemoryArea(E_INK_TypeDef *E_INK,
                          int16_t x_start, int16_t y_start, int16_t x_end, int16_t y_end);
void E_INK_SetMemoryPoint16_ter(E_INK_TypeDef *E_INK, int16_t x, int16_t y);
inline void E_INK_Delay(int8_t nDelay);

void E_INK_Init(E_INK_TypeDef *E_INK, const unsigned char *lut)
{
    E_INK->width = EPD_WIDTH;
    E_INK->height = EPD_HEIGHT;
    E_INK->lut = lut;
    
    // dc_pin, cs_pin  init
    GPIO_InitTypeDef Temp_GPIO_Structure = {
        .GPIO_Mode = GPIO_Mode_OUT,
        .GPIO_OType = GPIO_OType_PP,
        .GPIO_Speed = GPIO_Speed_50MHz,
        .GPIO_PuPd = GPIO_PuPd_UP,
    };
#ifdef USE_CS_PIN
    Temp_GPIO_Structure.GPIO_Pin = E_INK->cs_pin.Num;
    GPIO_Init(E_INK->cs_pin.Group, &Temp_GPIO_Structure);
    GPIO_SetBits(E_INK->cs_pin.Group, E_INK->cs_pin.Num);
#endif
    Temp_GPIO_Structure.GPIO_Pin = E_INK->dc_pin.Num;
    GPIO_Init(E_INK->dc_pin.Group, &Temp_GPIO_Structure);    
    GPIO_SetBits(E_INK->dc_pin.Group, E_INK->dc_pin.Num);

    
    // SPI Init
    SPI_InitTypeDef SPI_InitStructure = {
        .SPI_Direction = SPI_Direction_1Line_Tx,            //
        .SPI_Mode = SPI_Mode_Master,                        // 主 SPI
        .SPI_DataSize = SPI_DataSize_8b,                    // SPI 发送接收 8 位帧结构
        .SPI_CPOL = SPI_CPOL_High,                          // 串行同步时钟的空闲状态为低电平
        .SPI_CPHA = SPI_CPHA_2Edge,                         // 第一个跳变沿数据被采样
        .SPI_NSS = SPI_NSS_Soft,                            // NSS 信号由软件控制
        .SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2,   // 预分频2
        .SPI_FirstBit = SPI_FirstBit_MSB,                   // 数据传输从 MSB 位开始
        .SPI_CRCPolynomial = 7};                            // CRC值计算的多项式
    SPI_Init(E_INK->E_INK_SPI, &SPI_InitStructure);       // 根据指定的参数初始化外设 SPIx 寄存器
    SPI_CalculateCRC(E_INK->E_INK_SPI,DISABLE);
    SPI_Cmd(E_INK->E_INK_SPI, ENABLE);
        
    /* EPD hardware init start */
    E_INK_SendCommand(E_INK, DRIVER_OUTPUT_CONTROL);
    E_INK_SendData(E_INK, (EPD_HEIGHT - 1) & 0xFF);
    E_INK_SendData(E_INK, ((EPD_HEIGHT - 1) >> 8) & 0xFF);
    E_INK_SendData(E_INK, 0x00); // GD = 0; SM = 0; TB = 0;
    E_INK_SendCommand(E_INK, BOOSTER_SOFT_START_CONTROL);
    E_INK_SendData(E_INK, 0xD7);
    E_INK_SendData(E_INK, 0xD6);
    E_INK_SendData(E_INK, 0x9D);
    E_INK_SendCommand(E_INK, WRITE_VCOM_REGISTER);
    E_INK_SendData(E_INK, 0xA8); // VCOM 7C
    E_INK_SendCommand(E_INK, SET_DUMMY_LINE_PERIOD);
    E_INK_SendData(E_INK, 0x1A); // 4 dummy lines per gate
    E_INK_SendCommand(E_INK, SET_GATE_TIME);
    E_INK_SendData(E_INK, 0x08); // 2us per line
    E_INK_SendCommand(E_INK, DATA_ENTRY_MODE_SETTING);
    E_INK_SendData(E_INK, 0x03); // X increment; Y increment
    E_INK_SetLut(E_INK, E_INK->lut);
}


/*******basic function for sending commands********/
void E_INK_SendCommand(E_INK_TypeDef *E_INK, unsigned char command)
{
    GPIO_ResetBits(E_INK->dc_pin.Group, E_INK->dc_pin.Num);
    E_INK_Delay(SPI_TRANSMIT_DELAY);
#ifdef USE_CS_PIN
    GPIO_ResetBits(E_INK->cs_pin.Group, E_INK->cs_pin.Num);
#endif
    while (SPI_I2S_GetFlagStatus(E_INK->E_INK_SPI, SPI_I2S_FLAG_TXE) == RESET){}    //等待发送区空
    SPI_I2S_SendData(E_INK->E_INK_SPI, command);
    E_INK_Delay(SPI_TRANSMIT_DELAY);
#ifdef USE_CS_PIN
    GPIO_SetBits(E_INK->cs_pin.Group, E_INK->cs_pin.Num);
#endif
    E_INK_Delay(SPI_TRANSMIT_DELAY);
}

/*******basic function for sending data*******/
void E_INK_SendData(E_INK_TypeDef *E_INK, unsigned char data)
{
    GPIO_SetBits(E_INK->dc_pin.Group, E_INK->dc_pin.Num);
    E_INK_Delay(SPI_TRANSMIT_DELAY);
#ifdef USE_CS_PIN
    GPIO_ResetBits(E_INK->cs_pin.Group, E_INK->cs_pin.Num);
#endif
    while (SPI_I2S_GetFlagStatus(E_INK->E_INK_SPI, SPI_I2S_FLAG_TXE) == RESET){}    //等待发送区空
    SPI_I2S_SendData(E_INK->E_INK_SPI, data);
    E_INK_Delay(SPI_TRANSMIT_DELAY);
#ifdef USE_CS_PIN
    GPIO_SetBits(E_INK->cs_pin.Group, E_INK->cs_pin.Num);
#endif
    E_INK_Delay(SPI_TRANSMIT_DELAY);
}

/****** put an image buffer to the frame memory******/
void E_INK_SetFrameMemory(E_INK_TypeDef *E_INK,
                           const unsigned char *image_buffer,
                           int16_t x, int16_t y, int16_t image_width, int16_t image_height)
{
    int16_t x_end;
    int16_t y_end;

    if (image_buffer == NULL || x < 0 || image_width < 0 ||
        y < 0 || image_height < 0)
        return;
    /* x point16_t must be the multiple of 8 or the last 3 bits will be ignored */
    x &= 0xF8;
    image_width &= 0xF8;
    if (x + image_width >= E_INK->width)
        x_end = E_INK->width - 1;
    else
        x_end = x + image_width - 1;
    if (y + image_height >= E_INK->height)
        y_end = E_INK->height - 1;
    else
        y_end = y + image_height - 1;
    E_INK_SetMemoryArea(E_INK, x, y, x_end, y_end);
    E_INK_SetMemoryPoint16_ter(E_INK, x, y);
    E_INK_SendCommand(E_INK, WRITE_RAM);
    /* send the image data */
    for (int16_t j = 0; j < y_end - y + 1; j++)
        for (int16_t i = 0; i < (x_end - x + 1) / 8; i++)
            E_INK_SendData(E_INK, image_buffer[i + j * (image_width / 8)]);
}

/*******clear the frame memory with the specified color********/
void E_INK_ClearFrameMemory(E_INK_TypeDef *E_INK, unsigned char color)
{
    E_INK_SetMemoryArea(E_INK, 0, 0, E_INK->width - 1, E_INK->height - 1);
    E_INK_SetMemoryPoint16_ter(E_INK, 0, 0);
    E_INK_SendCommand(E_INK, WRITE_RAM);
    /* send the color data */
    for (int16_t i = 0; i < E_INK->width / 8 * E_INK->height; i++)
        E_INK_SendData(E_INK, color);
}

/**	 update the display
  *	 there are 2 memory areas embedded in the e-paper display
  *  but once this function is called
  *  the the next action of SetFrameMemory or ClearFrame will 
  *  set the other memory area
  */
void E_INK_DisplayFrame(E_INK_TypeDef *E_INK)
{
    E_INK_SendCommand(E_INK, DISPLAY_UPDATE_CONTROL_2);
    E_INK_SendData(E_INK, 0xC4);
    E_INK_SendCommand(E_INK, MASTER_ACTIVATION);
    E_INK_SendCommand(E_INK, TERMINATE_FRAME_READ_WRITE);
}

/********set the look-up tables********/
static void E_INK_SetLut(E_INK_TypeDef *E_INK, const unsigned char *lut)
{
    E_INK->lut = lut;
    E_INK_SendCommand(E_INK, WRITE_LUT_REGISTER);
    /* the length of look-up table is 30 bytes */
    for (int16_t i = 0; i < 30; i++)
        E_INK_SendData(E_INK, E_INK->lut[i]);
}

/******private function to specify the memory area for data R/W******/
static void E_INK_SetMemoryArea(E_INK_TypeDef *E_INK, 
                                 int16_t x_start, int16_t y_start, int16_t x_end, int16_t y_end)
{
    E_INK_SendCommand(E_INK, SET_RAM_X_ADDRESS_START_END_POSITION);
    /* x point16_t must be the multiple of 8 or the last 3 bits will be ignored */
    E_INK_SendData(E_INK, (x_start >> 3) & 0xFF);
    E_INK_SendData(E_INK, (x_end >> 3) & 0xFF);
    E_INK_SendCommand(E_INK, SET_RAM_Y_ADDRESS_START_END_POSITION);
    E_INK_SendData(E_INK, y_start & 0xFF);
    E_INK_SendData(E_INK, (y_start >> 8) & 0xFF);
    E_INK_SendData(E_INK, y_end & 0xFF);
    E_INK_SendData(E_INK, (y_end >> 8) & 0xFF);
}

/******private function to specify the start point16_t for data R/W******/
static void E_INK_SetMemoryPoint16_ter(E_INK_TypeDef *E_INK, int16_t x, int16_t y)
{
    E_INK_SendCommand(E_INK, SET_RAM_X_ADDRESS_COUNTER);
    /* x point16_t must be the multiple of 8 or the last 3 bits will be ignored */
    E_INK_SendData(E_INK, (x >> 3) & 0xFF);
    E_INK_SendCommand(E_INK, SET_RAM_Y_ADDRESS_COUNTER);
    E_INK_SendData(E_INK, y & 0xFF);
    E_INK_SendData(E_INK, (y >> 8) & 0xFF);
}

inline void E_INK_Delay(int8_t nDelay)
{
    while (nDelay-- > 0);
}

const unsigned char lut_full_update[] =
    {
        0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22,
        0x66, 0x69, 0x69, 0x59, 0x58, 0x99, 0x99, 0x88,
        0x00, 0x00, 0x00, 0x00, 0xF8, 0xB4, 0x13, 0x51,
        0x35, 0x51, 0x51, 0x19, 0x01, 0x00};

const unsigned char lut_partial_update[] =
    {
        0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x13, 0x14, 0x44, 0x12,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

