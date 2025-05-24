#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <math.h>

#define in3 18
#define in4 17


int main()
{
    stdio_init_all();
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    gpio_set_function(in3, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    gpio_set_function(in4, GPIO_FUNC_PWM);
    uint slice_num1 = pwm_gpio_to_slice_num(in3); // Get PWM slice number
    uint slice_num2 = pwm_gpio_to_slice_num(in4);
    float div = 60; // must be between 1-255
    pwm_set_clkdiv(slice_num1, div); // divider
    pwm_set_clkdiv(slice_num2, div);
    uint16_t wrap = 50000; // when to rollover, must be less than 65535
    pwm_set_wrap(slice_num1, wrap);
    pwm_set_wrap(slice_num2, wrap);
    pwm_set_enabled(slice_num1, true); // turn on the PWM
    pwm_set_enabled(slice_num2, true);

    float one, two, temp = 0;
    while(true){
        char msg;
        printf("Input + or -:\n");
        scanf(" %c", &msg);
        if(msg == '+'){
            temp = fmin(temp+0.01,1);
            printf("Current load: %f\n", temp);
        }else if (msg == '-'){
            temp = fmax(temp-0.01,-1);
            printf("Current load: %f\n", temp);
        }
        
        one = fmax(one, temp);
        two = fabs(fmin(two, temp));

        pwm_set_gpio_level(in3, wrap*one); // set the duty cycle to 50%
        pwm_set_gpio_level(in4, wrap*two);
        
    }

}
