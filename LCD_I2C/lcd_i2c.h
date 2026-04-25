/**
 * @file lcd_i2c.h
 * @date 4/21/2026
 * @author Cory W. Hodge
 * @brief API for LCD control over I2C bus
 */
#ifndef _LCD_I2C_H_
#define _LCD_I2C_H_

/*  -- INCLUDES -- */

// for i2c driver
#include "driver/i2c_master.h"


/*  -- DEFINES -- */

/**
 * Device Address : 0x27
 */
#define LCD_ADDR 0x27               // I2C address of the LCD

/**
 * LCD Clock Speed
 */
#define LCD_SCL_SPEED_HZ 100000     // I2C clock speed for the LCD

/**
 * 16 x 2 LCD Display 
 */
#define LCD_COLS 16                 // Number of columns in the LCD
#define LCD_ROWS 2                  // Number of rows in the LCD

/** 
 * Bit Mask for upper Nibble (D4-D7) connected to P4-P7 of the I2C expander
 */
#define EXPANDER_LCD_DATA_BIT_MASKS 0xF0

/**
 * The size of a nibble in bits
 */
#define NIBBLE_SIZE 4

/*  -- ENUMERATIONS -- */

/**
 * LCD Register Bit Masks for the I2C Expander
 * These bit masks correspond to the control bits used in the I2C expander to interface
 */
enum LCD_I2C_EXPANDER_REG_MASK
{
    LCD_I2C_EXPANDER_REG_REGSEL = 0x01,     // Bit 0: Register Select bit mask (0 = command, 1 = data)
    LCD_I2C_EXPANDER_REG_RW = 0x02,         // Bit 1: Read/Write bit mask (0 = write, 1 = read)
    LCD_I2C_EXPANDER_REG_ENABLE = 0x04,     // Bit 2: Enable bit mask (used to latch data)
    LCD_I2C_EXPANDER_REG_BACKLIGHT = 0x08   // Bit 3: Backlight bit mask (0 = off, 1 = on)
};

/**
 * LCD Entry Mode Register Bits
 */
enum LCD_EntryMode_Bits
{
    LCD_ENTRY_MODE_INCREMENT = 0x02, // Bit 1: Increment cursor position (1 = increment, 0 = decrement)
    LCD_ENTRY_MODE_SHIFT = 0x01      // Bit 0: Shift display (1 = shift, 0 = no shift)
};

/**
 * LCD Display Control Register Bits
 */
enum LCD_DisplayControl_Bits
{
    LCD_DISPLAY_CONTROL_DISPLAY = 0x04, // Bit 2: Display on/off (1 = display on, 0 = display off)
    LCD_DISPLAY_CONTROL_CURSOR = 0x02,  // Bit 1: Cursor on/off (1 = cursor on, 0 = cursor off)
    LCD_DISPLAY_CONTROL_BLINK = 0x01    // Bit 0: Blink on/off (1 = blink on, 0 = blink off)
};

/**
 * LCD Cursor/Display Shift Register Bits
 */
enum LCD_CursorDisplayShift_Bits
{
    LCD_CURSOR_DISPLAY_SHIFT_DISP_CURS= 0x08, // Bit 3: Shift display (1 = Display, 0 = Cursor)
    LCD_CURSOR_DISPLAY_SHIFT_LR = 0x04   // Bit 2: Shift cursor (1 = Right, 0 = Left)
};

/**
 * LCD Function Set Register Bits
 */
enum LCD_FunctionSet_Bits
{
    LCD_FUNCTION_SET_8BIT = 0x10,   // Bit 4: Data length (1 = 8-bit mode, 0 = 4-bit mode)
    LCD_FUNCTION_SET_2LINE = 0x08,  // Bit 3: Number of display lines (1 = 2 lines, 0 = 1 line)
    LCD_FUNCTION_SET_5x10 = 0x04    // Bit 2: Character font (1 = 5x10 dots, 0 = 5x8 dots)
};

/**
 * LCD Command Codes
 */
typedef enum LCD_Command
{
    LCD_CMD_CLEAR_DISPLAY = 0x01,
    LCD_CMD_RETURN_HOME = 0x02,
    LCD_CMD_ENTRY_MODE_SET = 0x04,
    LCD_CMD_DISPLAY_CONTROL = 0x08,
    LCD_CMD_DISP_CURS_SHIFT = 0x10,
    LCD_CMD_FUNCTION_SET = 0x20,
    LCD_CMD_SET_CGRAM_ADDR = 0x40,
    LCD_CMD_SET_DDRAM_ADDR = 0x80
} LCD_Command;


/* -- Structure Definitions -- */

/**
 * LCD Structure
 */
typedef struct
{
    // Handle for the I2C bus (to be used for communication with the LCD)
    i2c_master_bus_handle_t bus_handle;

    // Handle for the LCD device on the I2C bus 
    i2c_master_dev_handle_t lcd_handle;

} LCD_I2C;


/*  -- FUNCTION PROTOTYPES --  */

/**
 * LCD_I2C_Create
 * @param bus_handle The handle for the I2C bus (output parameter)
 * @param lcd_handle The handle for the LCD device on the I2C bus (output
 */
LCD_I2C * LCD_I2C_Create(i2c_master_bus_handle_t bus_handle, i2c_master_dev_handle_t lcd_handle);

/**
 * lcd_init
 * @param lcd The pointer for the LCD device on the I2C bus
 * This function initializes the LCD by following the specific power-up 
 * and initialization sequence required by the LCD controller.
 */
void lcd_init(LCD_I2C *  lcd);

/**
 * lcd_send_cmd 
 * @param lcd The pointer for the LCD device on the I2C bus
 * @param cmd The command byte to be sent to the LCD
 * This function sends a command byte to the LCD
 */
void lcd_send_cmd(LCD_I2C *  lcd, uint8_t cmd);

/**
 * lcd_send_data
 * @param lcd The pointer for the LCD device on the I2C bus
 * @param data The data byte to be sent to the LCD
 * This function sends a data byte to the LCD, which will be displayed on the screen.
 */
void lcd_send_data(LCD_I2C *  lcd, uint8_t data);

/**
 * lcd_print
 * @param lcd The pointer for the LCD device on the I2C bus
 * @param str The string to be printed on the LCD
 * This function prints a string on the LCD display.
 */
void lcd_print(LCD_I2C * lcd, const char * str);

/**
 * lcd_create_char
 * @param lcd The pointer for the LCD device on the I2C bus
 * Create a new character wilth custom pixel pattern and store it in the LCD's CGRAM
 */
void lcd_create_char(LCD_I2C * lcd, uint8_t location, const uint8_t charmap[]);

/**
 * lcd_home
 * @param lcd The pointer for the LCD device on the I2C bus
 */
void lcd_home(LCD_I2C * lcd);

/**
 * lcd_clear_display
 * @param lcd The pointer for the LCD device on the I2C bus
 */
void lcd_clear_display(LCD_I2C * lcd);

/**
 * lcd_set_cursor
 * @param lcd The pointer for the LCD device on the I2C bus
 * @param col The desired column position of the cursor
 * @param row The desired row position of the cursor
 * Sets the cursor to the position of col x row
 */
void lcd_set_cursor(LCD_I2C * lcd, uint8_t row, uint8_t col);

/**
 * lcd_display_on
 * @param lcd The pointer for the LCD device on the I2C bus
 * Turn  the display on
 */
void lcd_display_on(LCD_I2C * lcd);

/**
 * lcd_display_off
 * @param lcd The pointer for the LCD device on the I2C bus
 * Turn the display off
 */
void lcd_display_off(LCD_I2C * lcd);

/**
 * lcd_show_cursor
 * @param lcd The pointer for the LCD device on the I2C bus
 * Show the cursor on the LCD display
 */
void lcd_show_cursor(LCD_I2C * lcd);

/**
 * lcd_hide_cursor
 * @param lcd The pointer for the LCD device on the I2C bus
 * Hide the cursor on the LCD display
 */
void lcd_hide_cursor(LCD_I2C * lcd);

/**
 * lcd_blink_cursor
 * @param lcd The pointer for the LCD device on the I2C bus
 * Enable blinking of the cursor on the LCD display
 */
void lcd_blink_cursor(LCD_I2C * lcd);

/**
 * lcd_no_blink_cursor
 * @param lcd The pointer for the LCD device on the I2C bus
 * Disable blinking of the cursor on the LCD display
 */
void lcd_no_blink_cursor(LCD_I2C * lcd);

/**
 * lcd_display_shift_left
 * @param lcd The pointer for the LCD device on the I2C bus
 * @param units_left The number of positions to shift the display to the left
 * Shift the entire display to the left (without changing the cursor position)
 */
void lcd_display_shift_left(LCD_I2C * lcd, uint8_t units_left);

/**
 * lcd_display_shift_right
 * @param lcd The pointer for the LCD device on the I2C bus
 * @param units_right The number of positions to shift the display to the right
 * Shift the entire display to the right (without changing the cursor position)
 */
void lcd_display_shift_right(LCD_I2C * lcd, uint8_t units_right);

/**
 * lcd_cursor_shift_left
 * @param lcd The pointer for the LCD device on the I2C bus
 * @param units_left The number of positions to shift the cursor to the left
 * Shift the cursor to the left (without changing the display)
 */
void lcd_cursor_shift_left(LCD_I2C * lcd, uint8_t units_left);

/**
 * lcd_cursor_shift_right
 * @param lcd The pointer for the LCD device on the I2C bus
 * @param units_right The number of positions to shift the cursor to the right
 * Shift the cursor to the right (without changing the display)
 */
void lcd_cursor_shift_right(LCD_I2C * lcd, uint8_t units_right);

/**
 * lcd_display_enable_backlight
 * @param lcd The pointer for the LCD device on the I2C bus
 * Enable the backlight of the LCD display
 */
void lcd_display_enable_backlight(LCD_I2C * lcd);

/**
 * lcd_display_disable_backlight
 * @param lcd The pointer for the LCD device on the I2C bus
 * Disable the backlight of the LCD display
 */
void lcd_display_disable_backlight(LCD_I2C * lcd);


#endif // _LCD_I2C_H_