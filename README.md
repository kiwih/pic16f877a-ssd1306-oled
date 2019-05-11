# pic16f877a-ssd1306-oled
Making an SSD1306 work in the constrained PIC16F877A

This was a bit of a challenge. There's plenty of code online for driving the SSD1306, but most of it uses buffered memory (see the `_buffered.h` example for this).
But these don't work on a device like the PIC16F677A, because it just _doesn't have enough data memory_.

So what is a person to do?

Using the resources from 
* https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
* https://simple-circuit.com/ssd1306-oled-ccs-c-library/
* https://simple-circuit.com/pic18f4550-ssd1306-oled-display/

And then
* https://circuitdigest.com/microcontroller-projects/i2c-communication-with-pic-microcontroller-pic16f877a
for I2C,

I ported and edited and mangled the original source codes so that it would fit and support a subset of operations of the original SSD1306 driver operations on the I2C library, without any data buffering at all! Hooray!
(Wait, is this a good thing?)

If you're interested, the useful code is in `ssd1306_unbuffered.h` and `i2c.h`.

Anyway, it works! Woohoo!

![Demo image](demo.jpg)
