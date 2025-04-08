#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

void gpio_callback(uint gpio, uint32_t events) {
    // Put the GPIO event(s) that just happened into event_str
    // so we can print it
    gpio_put(14, false);
}

int main()
{
    stdio_init_all();
    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Start!\n");
    gpio_init(14);
    gpio_set_dir(14, GPIO_OUT);
    gpio_put(14, true);

    gpio_init(2);
    gpio_set_irq_enabled_with_callback(2, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);



    while (true) {
        printf("Enter a number between 0 and 100\n");
        int message;
        scanf("%d", &message);
        if(message>=0 && message < 101){
            printf("reading %d values\r\n", message);
            for(int i=0; i<message; i++){
                //printf("%d", i);
                uint16_t result = adc_read();
                printf("%f volts\n", (double) 3.3*result/4095);
                sleep_ms(10);
            }
        }

    }
}
