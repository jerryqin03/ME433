#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"
#include "hardware/adc.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

void drawLetter(int x, int y, char c);

void drawMessage(int x, int y, char* m);

void blink(char c);

int main()
{
    stdio_init_all();
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    stdio_init_all();
    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0);

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();

    double adcin;
    while (true) {
        //blink(0x22);
        unsigned int t1 = to_us_since_boot(get_absolute_time()); 
        char buf[20];
        char message[50];
        char fps[20];
        // sprintf(message,"hellohellohellohellohellohellohellohellohellohellohellohellohello");
        // drawMessage(0,0,message); //x,y,point to array
        adcin = 3.3*(double)adc_read()/4095;
        //printf("%f", adcin);
        sprintf(buf, "Voltage:");
        sprintf(message,"%.5f",adcin);
        
 
        drawMessage(0,0,buf);
        drawMessage(50,0,message);

        //ssd1306_update();
        unsigned int t2 = to_us_since_boot(get_absolute_time());  

        double frames = 1000000/((t2-t1));
        

        //write fps
        sprintf(fps, "FPS:");
        sprintf(fps+4, "%.5f", frames);
        drawMessage(0,10,fps);
        ssd1306_update();

    }
}

void drawMessage(int x, int y, char* m){
    int i = 0;
    while(m[i]!=0){
        // screen is 128x32
        drawLetter((x+i*5)%128, y + 8*((x+i*5)/128), m[i]);
        i++;
    }
}

void drawLetter(int x, int y, char c){
    int row, col;
    row = c-0x20;
    col = 0;
    
    for(col=0; col<5; col++){
        char byte = ASCII[row][col];

        for(int i=0; i<8; i++){
            char onoff = (byte>>i)&0b1;

            ssd1306_drawPixel(x+col, y+i, onoff);
        }
    }
}

void blink(char c){
    gpio_put(25, true);
    drawLetter(3,12,c);
    ssd1306_update();
    sleep_ms(250);
    ssd1306_clear();
    ssd1306_update();
    gpio_put(25, false);
    sleep_ms(250);


}


