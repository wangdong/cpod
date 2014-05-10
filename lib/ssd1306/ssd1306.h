#ifndef __H_ssd1306__
#define __H_ssd1306__

void ssd1306_init();
void ssd1306_puts_6x8 (int row, int col, char* s);
void ssd1306_puts_8x16(int row, int col, char* s);
void ssd1306_clear();

#endif