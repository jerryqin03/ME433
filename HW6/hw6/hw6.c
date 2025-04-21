#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

void initialize_i2c(){
    uint8_t ADDR = 0b0100000;
    uint8_t buf1[2], buf2[2];
    buf1[0] = 0x05;
    buf1[1] = 0b00100000;
    buf2[0] = 0x00;
    buf2[1] = 0b01111111;

    // get out of sequential mode
    i2c_write_blocking(i2c_default, ADDR, buf1, 2, false);
    gpio_put(25, true);
    sleep_ms(1000);
    gpio_put(25, false);
    sleep_ms(1000);

    // configure in/out of pins
    i2c_write_blocking(i2c_default, ADDR, buf2, 2, false);
    gpio_put(25, true);
    sleep_ms(1000);
    gpio_put(25, false);
    sleep_ms(1000);
}

void setPin(uint8_t address, uint8_t reg, uint8_t value){
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = value;
    i2c_write_blocking(i2c_default, address, buf, 2, false);
}

uint8_t readPin(){
    uint8_t read_buf, write_buf;
    int wt, rd;
    write_buf = 0x09;
    uint8_t read_address = 0b0100000;
    uint8_t write_address = 0b0100000;


    wt = i2c_write_blocking(i2c_default, read_address, &write_buf, 1, true);  // true to keep master control of bus
    rd = i2c_read_blocking(i2c_default, write_address, &read_buf, 1, false);  // false - finished with bus
    printf("write %i, read %i\n", wt, rd);

    return read_buf;
}

int main()
{
    stdio_init_all();
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    // gpio_pull_up(I2C_SDA);
    // gpio_pull_up(I2C_SCL);
    //bi_decl(bi_2pins_with_func(I2C_SDA, I2C_SCL, GPIO_FUNC_I2C));
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c


    initialize_i2c();

    uint8_t addr = 0b0100000;
    // uint8_t wr[2];
    // wr[0] = 0xA;
    // wr[1] = 0b10000000;
    uint8_t reg, val;
    reg = 0xA;
    val = 0b10000000;
    // i2c_write_blocking(i2c_default, addr, wr, 2, false);
    //setPin(addr, reg, val);
    gpio_put(25, true);
    sleep_ms(1000);
    gpio_put(25, false);
    sleep_ms(1000);

    uint8_t result;
    bool on = true;
    while (true) {
        uint8_t valt = 0b10000000;
        uint8_t valf = 0b00000000;
        result = readPin(); 
        result = (result << 7);
        if(result == 0){
            setPin(addr, reg, valt);
        }else{
            setPin(addr, reg, valf);
        }
        // if(on){
        //     setPin(addr, reg, valt);
        // }else{
        //     setPin(addr, reg, valf);
        // }
        printf("%i", result);
        gpio_put(25, true);
        sleep_ms(10);
        gpio_put(25, false);
        sleep_ms(10);
    }
    // start bit, address bits, read/write bit, information
    //          , 0100000     , 0/1,          , 

    // read data
    // i2c_write_blocking(i2c_default, ADDR, buf, 2, false);

    // write data
    // i2c_write_blocking(i2c_default, ADDR, &reg, 1, true);  // true to keep master control of bus
    // i2c_read_blocking(i2c_default, ADDR, &buf, 1, false);  // false - finished with bus
}
