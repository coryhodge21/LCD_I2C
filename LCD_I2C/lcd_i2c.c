/**
 * @file lcd_i2c.h
 * @date 4/21/2026
 * @author Cory W. Hodge
 * @brief Driver for controlling LCD display over I2C bus
 */

// Interface
#include "lcd_i2c.h"

// for FreeRTOS library delay functions
#include "freertos/FreeRTOS.h"

/*  -- CUSTOM CHARACTERS --  */

// See : https://maxpromer.github.io/LCD-Character-Creator/
//  for GUI to create custom characters and get the corresponding byte patterns

// Empty Circle
const uint8_t dotOff[8] = 
{
    0b00000,
    0b01110,
    0b10001,
    0b10001,
    0b10001,
    0b01110,
    0b00000, 
    0b00000
};

// Circle with a dot in the center
const uint8_t dotOn[8] = 
{
    0b00000,
    0b01110,
    0b11111,
    0b11111,
    0b11111,
    0b01110,
    0b00000, 
    0b00000
};

// Smiley Face Custom Character
const uint8_t smiley_face[8] = 
{
    0b00000,
    0b01010,
    0b01010,
    0b00000,
    0b10001,
    0b01110,
    0b00000,
    0b00000
};

const uint8_t heart[8] = 
{
    0b00000,
    0b00000,
    0b01010,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000
};

const uint8_t check_mark[8] = 
{
    0b00000,
    0b00001,
    0b00010,
    0b10100,
    0b01000,
    0b00000,
    0b00000,
    0b00000
};

const uint8_t X_mark[8] = 
{
    0b00000,
    0b10001,
    0b01010,
    0b00100,
    0b01010,
    0b10001,
    0b00000,
    0b00000
};

const uint8_t hour_glass[8] = 
{
    0b11111,
    0b10001,
    0b01110,
    0b00100,
    0b01010,
    0b10001,
    0b11111,
    0b00000
};

const uint8_t bell[8] = 
{
    0b00100,
    0b01110,
    0b01110,
    0b11111,
    0b11111,
    0b00000,
    0b00100,
    0b00000
};

const uint8_t music_note[8] = 
{
    0b00100,
    0b00110,
    0b00101,
    0b00100,
    0b01100,
    0b11100,
    0b01000,
    0b00000
};

/*  -- PRIVATE FUNCTION DEFINITIONS -- */

/**
 * lcd_write_nibble
 * @param lcd_handle The handle for the LCD device on the I2C bus
 * @param nibble The 4-bit data to be sent to the LCD (should be in the upper 4 bits of the byte)
 * @param mode The mode for the LCD (0 for command, 1 for data)
 * This function sends a single nibble (4 bits) to the LCD over I2C. 
 * It Toggles the Enable bit to latch the data and includes necessary delays for command execution.
 */
static void lcd_write_nibble(LCD_I2C * lcd, uint8_t nibble, uint8_t mode) 
{
    // Mask upper nibble OR with Backlight Bit OR with mode (data/cmd)
    uint8_t data = (nibble & LCD_I2C_EXPANDER_DATA_BIT_MASK)  | mode | LCD_I2C_EXPANDER_REG_BACKLIGHT;
    
    // Send data with Enable HIGH
    uint8_t byte_on = data | LCD_I2C_EXPANDER_REG_ENABLE;
    i2c_master_transmit(lcd->lcd_handle, &byte_on, 1, -1);
    
    // Small delay (Enable pulse width)
    esp_rom_delay_us(1); 
    
    // Send data with Enable LOW to latch
    uint8_t byte_off = data;
    i2c_master_transmit(lcd->lcd_handle, &byte_off, 1, -1);
    
    // Command execution delay
    esp_rom_delay_us(50); 
}

/** -- PUBLIC FUNCTION DEFINITIONS --  */

/**
 * LCD_I2C_Create
 * @param bus_handle The handle for the I2C bus (output parameter)
 * @param lcd_handle The handle for the LCD device on the I2C bus
 */
LCD_I2C * LCD_I2C_Create(i2c_master_bus_handle_t bus_handle, i2c_master_dev_handle_t lcd_handle)
{
    // Allocate memory for the LCD_I2C structure
    LCD_I2C * lcd = (LCD_I2C *)malloc(sizeof(LCD_I2C));

    // Check if memory allocation was successful
    if(lcd == NULL)
    {
        return NULL;
    }

    // Assign bus and device handles to the LCD structure
    lcd->bus_handle = bus_handle;
    lcd->lcd_handle = lcd_handle;

    // Return the pointer to the initialized LCD structure
    return lcd;
}

/**
 * lcd_init
 * @param lcd_handle The handle for the LCD device on the I2C bus
 * This function initializes the LCD by following the specific power-up 
 * and initialization sequence required by the LCD controller.
 */
void lcd_init(LCD_I2C * lcd)
{
    // Reference Initialization Sequence from HD44780 Datasheet (Section 3.3) and
    // Adafruit LCD Backpack Guide: https://learn.adafruit.com/adafruit-character
    const uint8_t init_sequence[] = {0x30, 0x30, 0x30, 0x20}; // Wake up sequence followed by setting 4-bit mode
    
    // Wait for VCC to stabilize (at least 40ms)
    esp_rom_delay_us(50000);

    /*The "Wake Up" sequence (Sent as single nibbles) */
    
    // Send 0x30 three times to reset the internal state machine
    lcd_write_nibble(lcd, init_sequence[0], 0);
    
    // Wait for more than 4.1ms after the first command
    esp_rom_delay_us(4500);
    
    // Send 0x30 again
    lcd_write_nibble(lcd, init_sequence[1], 0);

    // Wait for more than 100µs after the second command
    esp_rom_delay_us(200);
    
    // Send 0x30 a third time
    lcd_write_nibble(lcd, init_sequence[2], 0);

    // Wait for more than 100µs after the third command
    esp_rom_delay_us(200);

    // Set to 4-bit mode: After this command, 
	// 	the LCD will now expect two nibbles for every byte
    lcd_write_nibble(lcd, init_sequence[3], 0);

    // Wait for more than 100µs after setting 4-bit mode
    esp_rom_delay_us(200);

    // Entry Mode: Increment cursor
	const uint8_t entry_mode = LCD_CMD_ENTRY_MODE_SET | LCD_ENTRY_MODE_INCREMENT;
    lcd_send_cmd(lcd, entry_mode);

    // Function Set: 2-line, 5x8 dots
	const uint8_t function_set = LCD_CMD_FUNCTION_SET | LCD_FUNCTION_SET_2LINE;
    lcd_send_cmd(lcd, function_set);

    // Display ON, Cursor OFF
	const uint8_t display_control = LCD_CMD_DISPLAY_CONTROL | LCD_DISPLAY_CONTROL_DISPLAY;
    lcd_send_cmd(lcd, display_control);

    // Clear Display
	const uint8_t clear_display = LCD_CMD_CLEAR_DISPLAY;
    lcd_send_cmd(lcd, clear_display);

	// Wait for the clear display command to execute (takes about 1.5ms)
    esp_rom_delay_us(2000);

    /*  Initialize Custome Characters, memory location can be overwritten with new patterns */
    lcd_create_char(lcd, 0, dotOff);
    lcd_create_char(lcd, 1, dotOn);
    lcd_create_char(lcd, 2, smiley_face);
    lcd_create_char(lcd, 3, heart);
    lcd_create_char(lcd, 4, check_mark);
    lcd_create_char(lcd, 5, X_mark);
    lcd_create_char(lcd, 6, music_note);
    lcd_create_char(lcd, 7, hour_glass);
    // Note: Custom characters are stored in CGRAM locations 0-7, and can be printed by sending data bytes 0-7 to the LCD
}

/**
 * lcd_send_cmd 
 * @param lcd_handle The handle for the LCD device on the I2C bus
 * @param cmd The command byte to be sent to the LCD
 * This function sends a command byte to the LCD
 */
void lcd_send_cmd(LCD_I2C * lcd, uint8_t cmd)
{

    // Prepare High Nibble: Move bits 4-7 of data to bits 4-7 of the I2C byte
    lcd_write_nibble(lcd, cmd & LCD_I2C_EXPANDER_DATA_BIT_MASK, 0);       

    // Prepare Low Nibble: Move bits 0-3 of data to bits 4-7 of the I2C byte
    lcd_write_nibble(lcd, (cmd << NIBBLE_SIZE) & LCD_I2C_EXPANDER_DATA_BIT_MASK, 0); 
}

/**
 * lcd_send_data
 * @param lcd_handle The handle for the LCD device on the I2C bus
 * @param data The data byte to be sent to the LCD
 * This function sends a data byte to the LCD, which will be displayed on the screen.
 */
void lcd_send_data(LCD_I2C * lcd, uint8_t data)
{
    // Prepare High Nibble: Move bits 4-7 of data to bits 4-7 of the I2C byte
    lcd_write_nibble(lcd, (data & LCD_I2C_EXPANDER_DATA_BIT_MASK), LCD_I2C_EXPANDER_REG_REGSEL);

    // Prepare Low Nibble: Move bits 0-3 of data to bits 4-7 of the I2C byte
    lcd_write_nibble(lcd, ((data << NIBBLE_SIZE) & LCD_I2C_EXPANDER_DATA_BIT_MASK), LCD_I2C_EXPANDER_REG_REGSEL);
}

/**
 * lcd_print
 * @param lcd The pointer for the LCD device on the I2C bus
 * @param str The string to be printed on the LCD
 * This function prints a string on the LCD display.
 */
void lcd_print(LCD_I2C * lcd, const char * str)
{
    // Iterate through each character in the string and send it to the LCD
    while(*str)
    {
        // Send the current character as data to the LCD
        lcd_send_data(lcd, (uint8_t)(*str));

        // Move to the next character in the string
        str++;
    }
}

/**
 * lcd_create_char
 * @param lcd The pointer for the LCD device on the I2C bus
 * Create a new character wilth custom pixel pattern and store it in the LCD's CGRAM
 */
void lcd_create_char(LCD_I2C * lcd, uint8_t location, const uint8_t charmap[])
{
    // Ensure location is between 0 and 7 (CGRAM has 8 character slots)
    location &= 0x07;

    // Set CGRAM address to the desired location (each character takes 8 bytes)
    lcd_send_cmd(lcd, LCD_CMD_SET_CGRAM_ADDR | (location << 3));

    // Write the 8-byte character pattern to CGRAM
    for (int i = 0; i < 8; i++)
    {
        lcd_send_data(lcd, charmap[i]);
    }
}

/**
 * lcd_home
 * @param lcd The pointer for the LCD device on the I2C bus
 */
void lcd_home(LCD_I2C * lcd)
{
    lcd_send_cmd(lcd, LCD_CMD_RETURN_HOME);

    // Wait for the command to execute (takes about 1.5ms)
    esp_rom_delay_us(2000);
}

/**
 * lcd_clear_display
 * @param lcd The pointer for the LCD device on the I2C bus
 */
void lcd_clear_display(LCD_I2C * lcd)
{

    lcd_send_cmd(lcd, LCD_CMD_CLEAR_DISPLAY);

    // Wait for the clear display command to execute (takes about 1.5ms)
    esp_rom_delay_us(2000);
}

/**
 * lcd_set_cursor
 * @param lcd The pointer for the LCD device on the I2C bus
 * @param col The desired column position of the cursor
 * @param row The desired row position of the cursor
 * Sets the cursor to the position of col x row
 */
void lcd_set_cursor(LCD_I2C * lcd, uint8_t row, uint8_t col)
{
    // Calculate DDRAM address based on row and column
    uint8_t ddram_addr = (row == 0 ? 0x00 : 0x40) + col;

    // Send command to set DDRAM address (cursor position)
    lcd_send_cmd(lcd, LCD_CMD_SET_DDRAM_ADDR | ddram_addr);
}

/**
 * lcd_display_on
 * @param lcd The pointer for the LCD device on the I2C bus
 * Turn  the display on
 */
void lcd_display_on(LCD_I2C * lcd)
{
    // Read current display control settings (for simplicity, turn on the display without preserving other settings)
    const uint8_t display_control = LCD_CMD_DISPLAY_CONTROL | LCD_DISPLAY_CONTROL_DISPLAY;
    lcd_send_cmd(lcd, display_control);
}

/**
 * lcd_display_off
 * @param lcd The pointer for the LCD device on the I2C bus
 * Turn the display off
 */
void lcd_display_off(LCD_I2C * lcd)
{
    // Read current display control settings (for simplicity, turn off the display without preserving other settings)
    const uint8_t display_control = LCD_CMD_DISPLAY_CONTROL;
    lcd_send_cmd(lcd, display_control);
}

/**
 * lcd_show_cursor
 * @param lcd The pointer for the LCD device on the I2C bus
 * Show the cursor on the LCD display
 */
void lcd_show_cursor(LCD_I2C * lcd)
{
    // Read current display control settings (for simplicity, turn on the cursor without preserving other settings)
    const uint8_t display_control = LCD_CMD_DISPLAY_CONTROL | LCD_DISPLAY_CONTROL_DISPLAY | LCD_DISPLAY_CONTROL_CURSOR;
    lcd_send_cmd(lcd, display_control);
}

/**
 * lcd_hide_cursor
 * @param lcd The pointer for the LCD device on the I2C bus
 * Hide the cursor on the LCD display
 */
void lcd_hide_cursor(LCD_I2C * lcd)
{
    // Read current display control settings (for simplicity, turn off the cursor without preserving other settings)
    const uint8_t display_control = LCD_CMD_DISPLAY_CONTROL | LCD_DISPLAY_CONTROL_DISPLAY;
    lcd_send_cmd(lcd, display_control);
}

/**
 * lcd_blink_cursor
 * @param lcd The pointer for the LCD device on the I2C bus
 * Enable blinking of the cursor on the LCD display
 */
void lcd_blink_cursor(LCD_I2C * lcd)
{
    // Read current display control settings (for simplicity, turn on blinking without preserving other settings)
    const uint8_t display_control = LCD_CMD_DISPLAY_CONTROL | LCD_DISPLAY_CONTROL_DISPLAY | LCD_DISPLAY_CONTROL_BLINK;
    lcd_send_cmd(lcd, display_control);
}

/**
 * lcd_no_blink_cursor
 * @param lcd The pointer for the LCD device on the I2C bus
 * Disable blinking of the cursor on the LCD display
 */
void lcd_no_blink_cursor(LCD_I2C * lcd)
{
    // Read current display control settings (for simplicity, turn off blinking without preserving other settings)
    const uint8_t display_control = LCD_CMD_DISPLAY_CONTROL | LCD_DISPLAY_CONTROL_DISPLAY;
    lcd_send_cmd(lcd, display_control);
}

/**
 * lcd_display_shift_left
 * @param lcd The pointer for the LCD device on the I2C bus
 * Shift the entire display to the left (without changing the cursor position)
 */
void lcd_display_shift_left(LCD_I2C * lcd, uint8_t units_left)
{
    const uint8_t cursor_shift = LCD_CMD_DISP_CURS_SHIFT | LCD_CURSOR_DISPLAY_SHIFT_DISP_CURS | LCD_CURSOR_DISPLAY_SHIFT_LR;
    
    for(int i = 0; i < units_left; i++)
    {
        lcd_send_cmd(lcd, cursor_shift);
        esp_rom_delay_us(100); // Short delay between shifts
    }
}

/**
 * lcd_display_shift_right
 * @param lcd The pointer for the LCD device on the I2C bus
 * Shift the entire display to the right (without changing the cursor position)
 */
void lcd_display_shift_right(LCD_I2C * lcd, uint8_t units_right)
{
    const uint8_t cursor_shift = LCD_CMD_DISP_CURS_SHIFT | LCD_CURSOR_DISPLAY_SHIFT_DISP_CURS;
    
    for(int i = 0; i < units_right; i++)
    {
        lcd_send_cmd(lcd, cursor_shift);
        esp_rom_delay_us(100); // Short delay between shifts
    }
}

/**
 * lcd_display_scroll_left
 * @param lcd The pointer for the LCD device on the I2C bus
 * Scroll the entire display to the left (without changing the cursor position)
 */
void lcd_cursor_shift_left(LCD_I2C * lcd, uint8_t units_left)
{
    const uint8_t cursor_shift = LCD_CMD_DISP_CURS_SHIFT | LCD_CURSOR_DISPLAY_SHIFT_LR;

    for(int i = 0; i < units_left; i++)
    {
        lcd_send_cmd(lcd, cursor_shift);
        esp_rom_delay_us(100); // Short delay between shifts
    }
}

/**
 * lcd_display_scroll_right
 * @param lcd The pointer for the LCD device on the I2C bus
 * Scroll the entire display to the right (without changing the cursor position)
 */
void lcd_cursor_shift_right(LCD_I2C * lcd, uint8_t units_right)
{
    const uint8_t cursor_shift = LCD_CMD_DISP_CURS_SHIFT;

    for(int i = 0; i < units_right; i++)
    {
        lcd_send_cmd(lcd, cursor_shift);
        esp_rom_delay_us(100); // Short delay between shifts
    }
}

/**
 * lcd_display_enable_backlight
 * @param lcd The pointer for the LCD device on the I2C bus
 * Enable the backlight of the LCD display
 */
void lcd_display_enable_backlight(LCD_I2C * lcd)
{
    // To enable the backlight, we can send a command with the backlight bit set
    // Since the backlight is controlled by the I2C expander, we can simply send a command that includes the backlight bit
    const uint8_t backlight_on = LCD_I2C_EXPANDER_REG_BACKLIGHT;
    i2c_master_transmit(lcd->lcd_handle, &backlight_on, 1, -1);
}

/**
 * lcd_display_disable_backlight
 * @param lcd The pointer for the LCD device on the I2C bus
 * Disable the backlight of the LCD display
 */
void lcd_display_disable_backlight(LCD_I2C * lcd)
{
    // To disable the backlight, we can send a command with the backlight bit cleared
    const uint8_t backlight_off = 0x00; // No bits set means backlight off
    i2c_master_transmit(lcd->lcd_handle, &backlight_off, 1, -1);
}