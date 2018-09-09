#include "I2C_OLED.h"

static const uint16_t DELAY_600_NS = 120;
static const uint16_t DELAY_1_3_US = 240;

void I2C_OLED::Init(GPIO_TypeDef *SCL_pinGroup, uint16_t SCL_pinNum,
                    GPIO_TypeDef *SDA_pinGroup, uint16_t SDA_pinNum,
                    uint8_t address)
{
    SCL_pin.Group = SCL_pinGroup;
    SCL_pin.Num = SCL_pinNum;
    SDA_pin.Group = SDA_pinGroup;
    SDA_pin.Num = SDA_pinNum;
    /******  GPIO Init    ******/
    GPIO_InitTypeDef Temp_GPIO_Structure;
    Temp_GPIO_Structure.GPIO_Mode = GPIO_Mode_OUT;
    Temp_GPIO_Structure.GPIO_OType = GPIO_OType_PP;
    Temp_GPIO_Structure.GPIO_Speed = GPIO_Speed_100MHz;
    Temp_GPIO_Structure.GPIO_PuPd = GPIO_PuPd_UP;
    // Init SCL Pin:
    Temp_GPIO_Structure.GPIO_Pin = SCL_pin.Num;
    GPIO_Init(SCL_pin.Group, &Temp_GPIO_Structure);
    GPIO_SetBits(SCL_pin.Group, SCL_pin.Num);
    // Init SDA Pin:
    Temp_GPIO_Structure.GPIO_Pin = SDA_pin.Num;
    GPIO_Init(SDA_pin.Group, &Temp_GPIO_Structure);
    GPIO_SetBits(SDA_pin.Group, SDA_pin.Num);
    
    device_address = address;
    screenInit();
    clear();
}

/******  Basic Hardware Communication Function      ******/
void I2C_OLED::I2C_Start()
{
    uint16_t i = 0;
    GPIO_ResetBits(SDA_pin.Group, SDA_pin.Num);
    for (i=0; i < DELAY_600_NS; i++); // t_HSTART > 600ns
    GPIO_ResetBits(SCL_pin.Group, SCL_pin.Num);
    for (i=0; i < DELAY_600_NS; i++); // t_HSTART > 600ns
    WriteByte(device_address);
}

void I2C_OLED::I2C_Stop()
{    
    uint16_t i = 0;
    for (i=0; i < DELAY_600_NS; i++);    // t_SSTOP > 600ns
    GPIO_SetBits(SCL_pin.Group, SCL_pin.Num);
    GPIO_ResetBits(SDA_pin.Group, SDA_pin.Num);
    for (i=0; i < DELAY_600_NS; i++);    // t_SSTOP > 600ns    
    GPIO_SetBits(SDA_pin.Group, SDA_pin.Num);
    for (i = 0; i < DELAY_1_3_US; i++);  // t_IDLE > 1.3us(两次通信之间的时间间隔)
}

void I2C_OLED::sendCommand(uint8_t command)
{
    I2C_Start();
    WriteByte(0x80); //单次性写入命令
    WriteByte(command);
    I2C_Stop();
}

void I2C_OLED::sendData(uint8_t data)
{
    I2C_Start();
    WriteByte(0xC0); //单次性写入数据
    WriteByte(data);
    I2C_Stop();
}

void I2C_OLED::WriteByte(uint8_t data)
{
    uint16_t j = 0;
    for (uint8_t i = 0x80; i > 0; i >>= 1)
    {
        if (data & i)
            GPIO_SetBits(SDA_pin.Group, SDA_pin.Num);
        else
            GPIO_ResetBits(SDA_pin.Group, SDA_pin.Num);
        for (j = 0; j < DELAY_1_3_US; j++); //delay time > 1.25us

        GPIO_SetBits(SCL_pin.Group, SCL_pin.Num);
        for (j = 0; j < DELAY_1_3_US; j++); //delay time > 1.25us

        GPIO_ResetBits(SCL_pin.Group, SCL_pin.Num);
    }
    // wait ACK
    for (j = 0; j < DELAY_1_3_US; j++); //delay time > 1.25us
    GPIO_SetBits(SCL_pin.Group, SCL_pin.Num);
    for (j = 0; j < DELAY_1_3_US; j++); //delay time > 1.25us
    GPIO_ResetBits(SCL_pin.Group, SCL_pin.Num);
}

void I2C_OLED::setAddr(uint8_t seg_or_page, uint8_t startAddr, uint8_t endAddr)
{
    I2C_Start();
    WriteByte(0x00); //连续性写入命令
    WriteByte(seg_or_page);
    WriteByte(startAddr);
    WriteByte(endAddr);
    I2C_Stop();
}

void I2C_OLED::Refresh()
{
    buffer.column_end = buffer.column_end < OLED_WIDTH-1 ? buffer.column_end : OLED_WIDTH-1;
    buffer.row_end = buffer.row_end < OLED_HEIGHT - 1 ? buffer.row_end : OLED_HEIGHT - 1;
    if ((buffer.column_start > buffer.column_end) || (buffer.row_start > buffer.row_end)) return;

    uint8_t page_start = buffer.row_start >> 3, page_end = buffer.row_end >> 3;

    setAddr(OLED_COLUMN_ADDR, buffer.column_start, buffer.column_end);
    setAddr(OLED_PAGE_ADDR, page_start, page_end);

    uint8_t x = 0, y = 0;

    I2C_Start();
    WriteByte(0x40); //连续性写入数据
    for (y=page_start; y <= page_end; y++)
        for (x=buffer.column_start; x <= buffer.column_end; x++)
            WriteByte(buffer.GRAM[x + y * OLED_WIDTH]);
    I2C_Stop();

    buffer.column_start = 0;
    buffer.column_end = OLED_WIDTH - 1;
    buffer.row_start = 0;
    buffer.row_end = OLED_HEIGHT - 1;
}
