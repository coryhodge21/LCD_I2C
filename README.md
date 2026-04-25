Project : LCD_I2C <br>
Author : Cory W. Hodge <br>
Date : 4/25/2026 <br>
URL : [GitHub: coryhodge21](https://github.com/coryhodge21/LCD_I2C) <br>

# LCD_I2C
&emsp; This project is intended for the ESP32 Component Registery. It's an API/Driver for standard 16x2 LCD displays built upon HD44780U (LCD-II) IC paired with a standard PCF8574 i2c to 8-bit I/O Expander. This component is built upon the latest (as of Date) ESP32 ESP-IDF i2c driver ("driver/i2c_master").

### Quick Start

1. Create an LCD structure : LCD_I2C_Create()
2. Initialize the LCD Structure : lcd_init()
3. Send Commands/Data 
    - Direct byte by byte write : lcd_send_cmd() / lcd_send_data()
    - Simplified API for sending strings to display : lcd_print()
4. Utilize remaining API's for easy manipulation of display

### Developer Notes

- If you have issues with an LCD display, check your specific LCD's datasheet for the initialization sequence. This driver uses the sequence [0x30, 0x30, 0x30, 0x20] putting the lcd in 4-bit mode before any other operations are performed.
- The character creation feature provides 8 memory banks for custom characters (addressed as 0-7)
- This can be modified to support 16x4 lcd displays

## Resources
[ESP-IDF I2C api-reference](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/i2c.html) <br>

[HITACHI HD44780U LCD Data Sheet](https://circuitdigest.com/sites/default/files/HD44780U.pdf) <br> 

[TI PCF8574 Remote 8-Bit I/O Expander for I2C Bus](https://www.ti.com/lit/ds/symlink/pcf8574.pdf?ts=1777143071436&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FPCF8574%253Futm_source%253Dgoogle%2526utm_medium%253Dcpc%2526utm_campaign%253Dasc-null-null-GPN_EN-cpc-pf-google-ww_en_cons%2526utm_content%253DPCF8574%2526ds_k%253DPCF8574+Datasheet%2526DCM%253Dyes%2526gclsrc%253Daw.ds%2526gad_source%253D1%2526gad_campaignid%253D14388345080%2526gbraid%253D0AAAAAC068F0A4JlmWW3e-NX2qvFVCcZmm%2526gclid%253DCjwKCAjwg_nNBhAGEiwAiYPYA64ptTIfq_yJO3B1yX0jM_gFpHxiUGj-lfMgPbXX6abuBxpCi43hvxoCrQ8QAvD_BwE)
[]()