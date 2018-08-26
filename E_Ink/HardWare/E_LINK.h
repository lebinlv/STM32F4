#ifndef __E_INK_H
#define __E_INK_H
#include "stm32f4xx.h"
#include "stm32f4xx_spi.h"
#include "fonts.h"

#define USE_CS_PIN

// 屏幕相关命令定义
#define EPD_WIDTH 200
#define EPD_HEIGHT 200
#define DRIVER_OUTPUT_CONTROL                   0x01
#define BOOSTER_SOFT_START_CONTROL              0x0C
#define GATE_SCAN_START_POSITION                0x0F
#define DEEP_SLEEP_MODE                         0x10
#define DATA_ENTRY_MODE_SETTING                 0x11
#define SW_RESET                                0x12
#define TEMPERATURE_SENSOR_CONTROL              0x1A
#define MASTER_ACTIVATION                       0x20
#define DISPLAY_UPDATE_CONTROL_1                0x21
#define DISPLAY_UPDATE_CONTROL_2                0x22
#define WRITE_RAM                               0x24
#define WRITE_VCOM_REGISTER                     0x2C
#define WRITE_LUT_REGISTER                      0x32
#define SET_DUMMY_LINE_PERIOD                   0x3A
#define SET_GATE_TIME                           0x3B
#define BORDER_WAVEFORM_CONTROL                 0x3C
#define SET_RAM_X_ADDRESS_START_END_POSITION    0x44
#define SET_RAM_Y_ADDRESS_START_END_POSITION    0x45
#define SET_RAM_X_ADDRESS_COUNTER               0x4E
#define SET_RAM_Y_ADDRESS_COUNTER               0x4F
#define TERMINATE_FRAME_READ_WRITE              0xFF

#define COLORED 0
#define UNCOLORED 1

#define SPI_TRANSMIT_DELAY                      5

extern const unsigned char lut_full_update[];
extern const unsigned char lut_partial_update[];

typedef struct
{
    GPIO_TypeDef *Group;
    u16 Num;
} E_INK_Pin_TypeDef;


typedef struct
{
    E_INK_Pin_TypeDef dc_pin;
    E_INK_Pin_TypeDef cs_pin;
    SPI_TypeDef *E_INK_SPI;
    int16_t width;
    int16_t height;
    const unsigned char *lut;
} E_INK_TypeDef;

/* Hardware operating functions */
/* public functions */
void E_INK_Init(E_INK_TypeDef *E_INK, const unsigned char *lut);
void E_INK_SendCommand(E_INK_TypeDef *E_INK, unsigned char command);
void E_INK_SendData(E_INK_TypeDef *E_INK, unsigned char data);
void E_INK_WaitUntilIdle(E_INK_TypeDef *E_INK);
void E_INK_SetFrameMemory(E_INK_TypeDef *E_INK,
                           const unsigned char *image_buffer,
                           int16_t x, int16_t y, int16_t image_width, int16_t image_height);
void E_INK_ClearFrameMemory(E_INK_TypeDef *E_INK, unsigned char color);
void E_INK_DisplayFrame(E_INK_TypeDef *E_INK);


#endif
