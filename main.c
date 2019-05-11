/*
 * File:   main.c
 * Author: hammond
 *
 * Created on 11 May 2019, 2:56 PM
 */

#include "common.h"
#include "i2c.h"
#include "ssd1306_unbuffered.h"

void oled_puts(const char* c, uint8_t size) {
    while(*c != '\0') {
        SSD1306_PutStretchC(*c, size);
        c++;
    }
}

void main(void) {
    TRISD = 0x00;
    
    __delay_ms(1000);
    
    
    I2C_Initialize(2000);
    
    SSD1306_Init(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
    // clear the display
    SSD1306_ClearDisplay();
        
    
    while(1) {
        PORTD ^= 0xFF;
        
        SSD1306_GotoXY(1,1);
        oled_puts("I wanna be", 1);
        SSD1306_GotoXY(1,2);
        oled_puts("the", 2);
        SSD1306_GotoXY(5,2);
        oled_puts("very", 3);
        SSD1306_GotoXY(2,5);
        oled_puts("BEST", 4);
        __delay_ms(5000);
        SSD1306_ClearDisplay();
        
    }

    return;
}
