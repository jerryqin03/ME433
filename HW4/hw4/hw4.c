#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <math.h>

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   21
#define PIN_SCK  18
#define PIN_MOSI 19

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}

void write_dac(int channel, double voltage){
    uint8_t out[2];

    out[0] = 0;
    out[1] = 0;
    out[0] = out[0] | (channel<<7);
    out[0] = out[0] | (0b0111 << 4);

    uint16_t v = voltage*1024/3.3;

    out[0] = out[0] | (v>>6);
    out[1] = out[1] | (v<<2);


    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, out, 2); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS);
}

void make_data_sine(int channel, double frequency) {
    uint8_t out[2];
    float time = 0;
    double voltage;


    while(true){
        voltage = 1.65*sin(2*M_PI*frequency*time) + 1.65;
        write_dac(channel, voltage);

        time+= 1/(50*frequency);
        sleep_ms(1000/(50*frequency));

    }
}

void make_data_tri(int channel, double frequency) {
    double voltage;
    uint8_t out[2];
    float time = 0;

    while(true){
        voltage = fabs(3.3*(fmod(2*frequency*time, 2)-1));
        write_dac(channel, voltage);

        time+= 1/(500*frequency);
        sleep_ms(1000/(500*frequency));

    }
}



int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 10000000);
    gpio_init(PIN_CS);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    //gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    uint8_t test[2];
    test[0] = 0b01110111; 
    test[1] = 0b11111100;


    make_data_sine(0, 2);
    //make_data_tri(0,1);


}

