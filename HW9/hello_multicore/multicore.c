/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"

#define FLAG_VALUE 123
static double val01;
static double val10;

void core1_entry() {
    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0
    gpio_init(15);
    gpio_set_dir(15, GPIO_OUT);

    multicore_fifo_push_blocking(FLAG_VALUE);

    uint32_t g = multicore_fifo_pop_blocking();

    if (g != FLAG_VALUE)
        printf("Hmm, that's not right on core 1!\n");
    else
        printf("Its all gone well on core 1!\n");

    while (1){
        uint32_t g = multicore_fifo_pop_blocking();
        if(g == 0){
            uint16_t result = adc_read();
            val10 = (double) 3.3*result/4095;
            multicore_fifo_push_blocking(1);
        }else if(g == 1){
            gpio_put(15, true);
            multicore_fifo_push_blocking(2);
        }else if(g == 2){
            gpio_put(15,false);
            multicore_fifo_push_blocking(2);
            // led off
        }else{
            multicore_fifo_push_blocking(0);
        }
    }
}

int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Hello, multicore!\n");

    /// \tag::setup_multicore[]

    multicore_launch_core1(core1_entry);
    multicore_fifo_push_blocking(FLAG_VALUE);

    // Wait for it to start up

    uint32_t g = multicore_fifo_pop_blocking();

    if (g != FLAG_VALUE)
        printf("Hmm, that's not right on core 0!\n");
    else {
        multicore_fifo_push_blocking(FLAG_VALUE);
        printf("It's all gone well on core 0!\n");
    }

    while (1){
        int in;
        printf("Input 0,1,or 2:\n");
        scanf("%i", &in);
        printf("You typed: %i\n", in);
        multicore_fifo_push_blocking(in);
        uint32_t g = multicore_fifo_pop_blocking();
        if(g == 1){
            printf("Voltage: %f\n", val10);
        }

    }

    /// \end::setup_multicore[]
}
