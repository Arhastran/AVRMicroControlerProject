#ifndef lcd_lib 
#define lcd_lib
void lcd_write_byte(uint8_t data);
void lcd_write_cmd(uint8_t cmd);
void lcd_write_data(uint8_t data);
void lcdinit(void);
void lcd_clr(void);
void lcd_cursor_return(void);
void lcd_text(char *str);
void lcd_int(int16_t z);
void goto_xy (uint8_t x, uint8_t y);
void lcd_image(void);
#endif
