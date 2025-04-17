#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <math.h>

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS_RAM  17
#define PIN_CS_DAC  21
#define PIN_SCK  18
#define PIN_MOSI 19
#define frequency 1

union FloatInt{
    float f;
    uint32_t i;
};

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


    cs_select(PIN_CS_DAC);
    spi_write_blocking(SPI_PORT, out, 2); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS_DAC);
}

void spi_ram_init(){
    uint8_t buf[2];
    buf[0] = 0b00000001;
    buf[1] = 0b01000000;
    cs_select(PIN_CS_RAM);
    spi_write_blocking(spi_default, buf, 2);
    cs_deselect(PIN_CS_RAM);
}

void spi_ram_write(uint16_t addr, float v){
    uint8_t buf[7];
    buf[0] = 0b00000010;
    buf[1] = addr>>8;
    buf[2] = addr&0xFF;

    union FloatInt num;
    num.f = v;

    buf[3] = (num.i>>24)&0xFF;
    buf[4] = (num.i>>16)&0xFF;
    buf[5] = (num.i>>8)&0xFF;
    buf[6] = num.i&0xFF;

    cs_select(PIN_CS_RAM);
    spi_write_blocking(spi_default, buf, 7);
    cs_deselect(PIN_CS_RAM);
}

float spi_ram_read(uint16_t addr){
    uint8_t write[7], read[7];
    write[0] = 0b00000011;
    write[1] = (addr>>8)&0xFF;
    write[2] = addr&0xFF;

    cs_select(PIN_CS_RAM);
    spi_write_read_blocking(spi_default, write, read, 7);
    cs_deselect(PIN_CS_RAM);

    // read[0]
    // read[1]
    // read[2]

    union FloatInt num;
    num.i = num.i | (read[3]<<24);
    num.i = num.i | (read[4]<<16);
    num.i = num.i | (read[5]<<8);
    num.i = num.i | read[6];
    
    return num.f;
}

int main()
{
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    volatile float f1, f2;
    printf("Enter two floats to use:");
    scanf("%f %f", &f1, &f2);
    volatile float f_add, f_sub, f_mult, f_div;
    absolute_time_t t0 = get_absolute_time();
    for(int i=0; i<1000; i++){
        f_add = f1+f2;
    }
    absolute_time_t t1 = get_absolute_time();
    for(int j=0; j<1000; j++){
        f_sub = f1-f2;
    }
    absolute_time_t t2 = get_absolute_time();
    for(int k=0; k<1000; k++){
        f_mult = f1*f2;
    }
    absolute_time_t t3 = get_absolute_time();
    for(int l=0; l<1000; l++){
        f_div = f1/f2;
    }
    absolute_time_t t4 = get_absolute_time();

    uint64_t t_0 = to_us_since_boot(t0);
    uint64_t t_1 = to_us_since_boot(t1);
    uint64_t t_2 = to_us_since_boot(t2);
    uint64_t t_3 = to_us_since_boot(t3);
    uint64_t t_4 = to_us_since_boot(t4);
    uint64_t add = (t_1-t_0)/6.667;
    uint64_t sub = (t_2-t_1)/6.667;
    uint64_t mul = (t_3-t_2)/6.667;
    uint64_t div = (t_4-t_3)/6.667;

    printf("\nResults: \nAddition takes %llu clock cycles \nSubtraction takes %llu clock cycles \nMultiplication takes %llu clock cycles \nDivision takes %llu clock cycles\n", add, sub, mul, div);


    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS_RAM,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_CS_DAC,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS_RAM, GPIO_OUT);
    gpio_set_dir(PIN_CS_DAC, GPIO_OUT);
    gpio_put(PIN_CS_RAM, 1);
    gpio_put(PIN_CS_DAC, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi
    // write sine data to the RAM
    spi_ram_init();
    float sine;
    uint16_t addr = 0;
    for(int i=0; i<1000; i++){
        sine = 1.65*sin(2*M_PI*frequency*i/1000) + 1.65;
        spi_ram_write(addr, sine);
        addr=addr+4;
    }

    // read data from RAM, send to DAC
    addr = 0;
    while(true){
        float s = spi_ram_read(addr);
        write_dac(0, s);
        addr=addr+4;
        if(addr>=4000){
            addr=0;
        }
        sleep_ms(1);
        
    }



    
}
