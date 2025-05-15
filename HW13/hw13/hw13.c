#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <math.h>

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9
// config registers
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
// sensor data registers:
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define WHO_AM_I     0x75
#define ADDR 0x68

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

void drawLetter(int x, int y, char c);

void drawMessage(int x, int y, char* m);

void blink(char c);

void imu_init();

void imu_read(double* in);

void drawline(double *in);

int main()
{
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    stdio_init_all();
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c
    imu_init();
    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();

    // double data[3] = {1,3,2};
    // drawline(data);
    // ssd1306_update();
    
    while(true){
        ssd1306_clear();
        sleep_ms(10);
        double data[7];
        imu_read(data);
        drawline(data);
        ssd1306_update();
    }

}

void drawline(double *in){
    double x,xstart,xend,y,ystart,yend;
    double xlist[16], ylist[16];
    x = in[0] - 1;
    y = in[1];
    xend = 16*x;///sqrt(x*x + y*y);
    yend = 16*y;///sqrt(x*x + y*y);
    xstart = 64;
    ystart = 16;
    printf("s%f,%f\n",xend, yend);

    for(int i=0; i<16; i++){
        xlist[i] = xstart + xend*((double)i/16);
        ylist[i] = ystart + yend*((double)i/16);
        printf("%f, %f\n", xlist[i], ylist[i]);
    }

    for(int j=0; j<16; j++){
        ssd1306_drawPixel(round(xlist[j]), round(ylist[j]), 1);
    }

    
}

void imu_init(){
    uint8_t buf[1];
    uint8_t who = WHO_AM_I;
    uint8_t pwron[2];
    pwron[0] = PWR_MGMT_1;
    pwron[1] = 0x00;
    uint8_t accel_config[2];
    accel_config[0] = ACCEL_CONFIG;
    accel_config[1] = 0b00000000;
    uint8_t gyro_config[2];
    gyro_config[0] = GYRO_CONFIG;
    gyro_config[1] = 0b00011000;

    i2c_write_blocking(i2c_default, ADDR, &who, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, ADDR, buf, 1, false);  // false - finished with bus
    
    if (buf[0] != 0x68){
        printf("%02X", buf[0]);
        while(true){
            gpio_put(25, 1);
        }
    }

    i2c_write_blocking(i2c_default, ADDR, pwron, 2, false);
    i2c_write_blocking(i2c_default, ADDR, accel_config, 2, false);
    i2c_write_blocking(i2c_default, ADDR, gyro_config, 2, false);

    

    // i2c_write_blocking(i2c_default, PWR_MGMT_1, &pwron, 1, false);  // true to keep master control of bus
    // i2c_write_blocking(i2c_default, ACCEL_CONFIG, &accel_config, 1, false);
    // i2c_write_blocking(i2c_default, GYRO_CONFIG, &gyro_config, 1, false);
}

void imu_read(double* in){
    //uint8_t start[] = {ACCEL_XOUT_H, ACCEL_XOUT_L, ACCEL_YOUT_H, ACCEL_YOUT_L, ACCEL_ZOUT_H, ACCEL_ZOUT_L, TEMP_OUT_H, TEMP_OUT_L, GYRO_XOUT_H, GYRO_XOUT_L, GYRO_YOUT_H, GYRO_YOUT_L, GYRO_ZOUT_H, GYRO_ZOUT_L};
    uint8_t start = ACCEL_XOUT_H;
    uint8_t out[14];
    i2c_write_blocking(i2c_default, ADDR, &start, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, ADDR, out, 14, false);

    for(int i=0; i<7; i++){
        int16_t temp = (out[2*i] << 8) | (out[2*i + 1]);
        // printf("%d\n", temp);

        if(i<3){
            printf("%i,%f\n", i, 0.000061*temp);
            in[i] = 0.000061*temp;
        }else if(i==3){
            printf("%i,%f\n", i, temp/340.00+36.53);
            in[i] = temp/340.00+36.53;
        }else{
            printf("%i,%f\n", i, temp*0.007630);
            in[i] = temp*0.007630;
        }
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





