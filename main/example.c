/**
 * @file
 * @author: Cory W. Hodge
 * @date: 2024-06-01
 * @brief: Example C11 code demonstrating basic syntax and features.
 */

// for FreeRTOS library
#include "freertos/FreeRTOS.h"

// for i2c driver
#include "driver/i2c_master.h"

// for ESP_ERROR_CHECK() and esp_rom_delay_us()
#include <esp_err.h>

// for LCD_I2C library
#include "LCD_I2C.h"


void app_main(int argc, char *argv[]) {

    // Master Bus Handle
    i2c_master_bus_handle_t bus_handle = NULL;
    
    // LCD Device Handle ( device to be added to the master bus)
    i2c_master_dev_handle_t lcd_handle = NULL;

   /* -- Set up the I2C Bus -- */

   // Master Bus Configurations
   i2c_master_bus_config_t i2c_mst_config = 
   {
       .clk_source = I2C_CLK_SRC_DEFAULT,
       .i2c_port = I2C_NUM_0,
       .scl_io_num = GPIO_NUM_22,
       .sda_io_num = GPIO_NUM_21,
       .glitch_ignore_cnt = 7,
       .flags.enable_internal_pullup = true,
   };

   // Install Master Bus
   ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

   /* -- Add LCD Display to the I2C Bus -- */

    // Slave Device Configurations
    i2c_device_config_t dev_cfg = 
    {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = LCD_ADDR,
        .scl_speed_hz = LCD_SCL_SPEED_HZ,
    };
    
    // Install LCD/I2C Expander Device on Master Bus
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &lcd_handle));

    /* -- LCD -- */

    // Create LCD Object
    LCD_I2C * lcd = LCD_I2C_Create(bus_handle, lcd_handle);

    // Initialize LCD
    lcd_init(lcd);

	// Prepare the LCD screen
    lcd_send_cmd(lcd, LCD_CMD_CLEAR_DISPLAY); 
    lcd_send_cmd(lcd, LCD_CMD_RETURN_HOME);  

	/* Task Loop */
    while(1)
    {
        // Test cursor
        lcd_home(lcd);  // Move cursor to home position (0,0)
        lcd_show_cursor(lcd);
        lcd_blink_cursor(lcd);

        // Test Sent Bytes
		lcd_send_data(lcd, 'H');
		vTaskDelay(500 / portTICK_PERIOD_MS);

		lcd_send_data(lcd, 'E');
		vTaskDelay(500 / portTICK_PERIOD_MS);

		lcd_send_data(lcd, 'L');
		vTaskDelay(500 / portTICK_PERIOD_MS);

        lcd_send_data(lcd, 'L');
		vTaskDelay(500 / portTICK_PERIOD_MS);

        lcd_send_data(lcd, '0');
		vTaskDelay(500 / portTICK_PERIOD_MS);

        // Test Print String
        lcd_print(lcd, " RIO");

        // Test No Blink Cursor
        lcd_no_blink_cursor(lcd);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // Test set cursor
        lcd_set_cursor(lcd, 1, 3);  // Move cursor to the beginning of the second line
        lcd_print(lcd, "Woof Woof!");
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // test lcd_send_cmd fx
		lcd_send_cmd(lcd, LCD_CMD_CLEAR_DISPLAY);
		lcd_send_cmd(lcd, LCD_CMD_RETURN_HOME);  
		vTaskDelay(1000 / portTICK_PERIOD_MS);


        // Test Custom Characters
        lcd_print(lcd, "Custom Chars: ");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        
		lcd_set_cursor(lcd, 1, 3);  // Move cursor to the middle of the second line

        // Test hide cursor
        lcd_hide_cursor(lcd);

        for(int i = 0; i < 7; i++)
        {
            lcd_send_data(lcd, i); // Print custom characters (0-7)
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        // Test Shifts
        lcd_display_shift_left(lcd, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        lcd_display_shift_left(lcd, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        lcd_display_shift_left(lcd, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);


        lcd_display_shift_right(lcd, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        lcd_display_shift_right(lcd, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        lcd_display_shift_right(lcd, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);

        // Test Display Cycle
        lcd_display_off(lcd);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        lcd_display_on(lcd);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        lcd_display_off(lcd);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        lcd_display_on(lcd);
        vTaskDelay(500 / portTICK_PERIOD_MS);

        // Test Display On/Off
        lcd_display_disable_backlight(lcd);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        lcd_display_enable_backlight(lcd);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        lcd_display_disable_backlight(lcd);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        lcd_display_enable_backlight(lcd);
        vTaskDelay(500 / portTICK_PERIOD_MS);

        // Test command wrappers
        lcd_home(lcd);
	    lcd_clear_display(lcd);  

        // Short delay before repeating the loop
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}