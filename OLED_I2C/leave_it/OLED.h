#ifndef OLED_I2C_H_
#define OLED_I2C_H_

#include "stm32f4xx.h"
#include "DisplayFonts.h"

#ifndef MIN
#define MIN(A, B) (A)<=(B)?(A):(B)
#endif

#ifndef MAX
#define MAX(A, B) (A)>=(B)?(A):(B)
#endif

#ifndef __OLED__
#define __OLED__
// Display commands
#define OLED_CHARGE_PUMP            0x8D
#define OLED_COLUMN_ADDR            0x21
#define OLED_COM_SCAN_C0            0xC0
#define OLED_COM_SCAN_C8            0xC8
#define OLED_DISPLAY_ALL_ON         0xA5
#define OLED_DISPLAY_ALLON_RESUME   0xA4
#define OLED_DISPLAY_OFF            0xAE
#define OLED_DISPLAY_ON             0xAF
#define OLED_EXTERNAL_VCC           0x01
#define OLED_INVERT_DISPLAY         0xA7
#define OLED_MEMORY_MODE            0x20
#define OLED_NORMAL_DISPLAY         0xA6
#define OLED_PAGE_ADDR              0x22
#define OLED_SEG_REMAP_A0           0xA0
#define OLED_SEG_REMAP_A1           0xA1
#define OLED_SET_COMPINS            0xDA
#define OLED_SET_CONTRAST           0x81
#define OLED_SET_DCLK_CLK           0xD5
#define OLED_SET_DISPLAY_OFFSET     0xD3
#define OLED_SET_HIGH_COLUMN        0x10
#define OLED_SET_LOW_COLUMN         0x00
#define OLED_SET_MUX_Ratio          0xA8
#define OLED_SET_PRE_CHARGE         0xD9
#define OLED_SET_START_LINE         0x40
#define OLED_SET_VCOMH_DESELECT     0xDB
#define OLED_SWITCH_CAP_VCC         0x02

// 建立结构体封装引脚的组和编号；
struct Pin_Struct
{
    GPIO_TypeDef *Group;
    uint16_t Num;
};


struct Display_Buffer
{
    uint8_t *GRAM;         // pointer, point to display buffer
    uint8_t column_start;  // the start column of modified data, 0<=column_start<128
    uint8_t column_end;    // the end column of modified data, column_start<=column_end<128
    uint8_t page_start;    // the start page of modified data, 0<=page_start<8
    uint8_t page_end;      // the end page of modified data, page_start<=page_end<8
};
#endif

class I2C_OLED
{
private:
    static const uint8_t OLED_WIDTH = 128;              // screen width
    static const uint8_t OLED_HEIGHT = 64;              // screen height

    fontStruct Font = SansSerif_plain_12;

    enum {DELAY_600_NS = 120, DELAY_1_3_US = 240};      //used to control delay;

    Pin_Struct SCL_pin;     // SCL pin Struct;
    Pin_Struct SDA_pin;     // SDA pin Struct;

    uint8_t temp[1024];     // display data buffer;
    uint8_t device_address; // device address, 0x78 in default;

    Display_Buffer buffer;



    void I2C_Start();       // send I2C start signal and address byte
    void I2C_Stop();        // send I2C stop signal
    void WriteByte(uint8_t data);   // write a byte

public:

    /**
     * @brief: drawMode Enumeration type:
     *        NORMAL: draw the element in normal mode;
     *         CLEAR: use the element to clear a area;
     *       INVERSE: use the element to inverse a area;
     * */
    enum drawMode { NORMAL, CLEAR, INVERSE };
    
    // default constructor function, you must provide parameters to canstructor init the class;
    I2C_OLED(GPIO_TypeDef * SCL_pinGroup, uint16_t SCL_pinNum,
                GPIO_TypeDef *SDA_pinGroup, uint16_t SDA_pinNum,
                uint8_t address = 0x78);

    ~I2C_OLED();

    // hardware init function.
    void Init();

    /**
     * @brief: clear screen;
     * @param: [update]: optional parameter.
     *    true(default): clear GRAM and refresh screen;
     *            false: clear GRAM only.
     * */
    void clear(bool update = true);

    //default screen refresh function. 
    void Refresh();

    /**
     * @brief: overloaded refresh function, 
     *         refresh the specified area from (x_start, y_start) to (x_end, y_end);
     * @param:   x_start: >=0 and <=127;
     * @param:     x_end: >=x_start and <=127;
     * @param:   y_start: >=0 and <=63;
     * @param:     y_end: >= y_start and <=63;
     **/
    void Refresh(uint8_t x_start, uint8_t x_end, uint8_t y_start, uint8_t y_end);

    // you can use this function to change buffer.GRAM
    void setBuffer(uint8_t * pBuffer);
    // reset the buffer.GRAM to internal buffer
    void resetBuffer();

    /******      HardWare Operate Function      ******/
    // send a single command
    void sendCommand(uint8_t command);

    // send a single data
    void sendData(uint8_t data);

    /**
     * @brief: Setup column or page start and end address, only for horizontal or vertical addressing mode;
     * @param: seg_or_page: it can be OLED_COLUMN_ADDR or OLED_PAGE_ADDR;
     * @param:   startAddr: start address,
     *                      when seg_or_page==OLED_COLUMN_ADDR, >=0 and <=127,
     *                      when seg_or_page==OLED_PAGE_ADDR, >=0 and <=7;
     * @param:     endAddr: end address,
     *                      when seg_or_page==OLED_COLUMN_ADDR, >=startAddr and <=127,
     *                      when seg_or_page==OLED_PAGE_ADDR, >=startAddr and <=7;
     * */
    inline void setAddr(uint8_t seg_or_page, uint8_t startAddr, uint8_t endAddr)
    {
        I2C_Start();
        WriteByte(0x00);        //连续性写入命令
        WriteByte(seg_or_page);
        WriteByte(startAddr);
        WriteByte(endAddr);
        I2C_Stop();
    }



    /******      Basic Display Control Function      ******/
    void turnOnScreen();    // turn on the screen;
    void turnOffScreen();   // turn off the screen, just like "power save mode";
    void invert_mode();     // set the OLED to invert display mode: bit 0 in GRAM indicates an "ON" pixel;
    void normal_mode();     // (default mode)set the OLED to normal display mode: bit 1 in GRAM indivates an "ON" pixel;
    void setContrast(uint8_t contrast); // adjust the contrast, 0<=contrast<=0xff;
    void setScreen_UpsideDown();        // Turn the display upside down
    void resetScreen_Orientation();     // (default mode) take the position of pins as screen's top


    /******      Basic Drawing Function      ******/

    // Draw a pixel. just changes the data stored in GRAM, you should refresh the screen manually.
    void setColoredPixel(uint8_t x, uint8_t y);     // turn on a pixel;
    void setUncoloredPixel(uint8_t x, uint8_t y);   // turn off a pixal;
    void setInversePixel(uint8_t x, uint8_t y);     // inverse a pixel;

    // Draw baxic Geometric patterns
    /**
     * @brief: Draw a line from position(x0,y0) to position (x1,y1);
     * @param: update: 
     *              true(default): draw a line in GRAM and refresh screeen;
     *                      false: draw a line in GRAM and change modified area information that stored in buffer structure,
     *                        you can use refresh() function to refrsh the screen manually.
     * */
    void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, bool update = true);

    /**
     * @brief: Draw a horizontal line.
     * @param:    x,y: start point (x,y);
     * @param: length: length of the line;
     * @param: update: it must be "true", just leave it;
     * @param:   mode: it can be NORMAL, CLEAR, INVERSE, just leave it;
     * */
    void drawHorizontalLine(int16_t x, int16_t y, uint8_t length, bool update = true, drawMode mode = NORMAL);

    // Draw a vertical line.
    void drawVerticalLine(int16_t x, int16_t y, uint8_t length, bool update = true, drawMode mode = NORMAL);

    // Draw the border of a rectangle at the given location
    void drawRect(int16_t x, int16_t y, uint8_t width, uint8_t height, bool update = true);

    /**
     * @brief: draw a filled rectangle or use this rectangle to realise clear or inverse a specified area.
     * @param    mode: it can be NORMAL, CLEAR, INVERSE:
     *                 NORMAL: normal display mode, draw a filled rectangle;
     *                  CLEAR: use this rectangle to clear a area;
     *                INVERSE: use this rectangle to inverse a area;
     * */
    void drawFilledRect(int16_t x, int16_t y, uint8_t width, uint8_t height, drawMode mode = NORMAL, bool update = true);
    
    // Draw the border of a circle
    void drawCircle(int16_t x0, int16_t y0, uint8_t radius, bool update = true);

    // draw a filled circle or use this circle to realise clear or inverse a specified area.
    void drawFilledCircle(int16_t x0, int16_t y0, uint8_t radius, drawMode mode = NORMAL, bool update = true);


    // Basic test operations
    void setFont(const fontStruct newFont);
    uint16_t getStringWidth(const char *text, uint16_t length);
    uint16_t getStringWidth(const char* userStr);
    void inline drawChar(int16_t x, int16_t y, uint8_t width, uint8_t height, uint16_t offset, uint16_t sizeOfChar);
    void drawStringInternal(int16_t x, int16_t y, const char *text, uint8_t textLength, uint8_t textWidth);
    void drawString(int16_t x, int16_t y, char *usrStr);
};

#endif
