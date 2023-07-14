#include "debug.h"

/* C Includes */
#ifdef DEBUG
#include <stdio.h>
#endif

/* Pico SDK Includes */

/* Project Includes */
#include "core_0.h"
#include "core_1_entry.h"

int main()
{   
    stdio_init_all();
    sleep_ms(5000);

    i2c_init(i2c0, 400000);
    i2c_init(i2c1, 400000);

#ifdef DEBUG
    printf("<0> I2C1 Init Done\n");fflush(stdout);
    fflush(stdout);
#endif

    /* GPIO Configuration */
    gpio_init(PICO_LED);
    gpio_set_dir(PICO_LED, true);

    gpio_set_function(I2C0_DATA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C0_CLOCK_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C0_DATA_PIN);
    gpio_pull_up(I2C0_CLOCK_PIN);

    
    gpio_set_function(I2C1_DATA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C1_CLOCK_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C1_DATA_PIN);
    gpio_pull_up(I2C1_CLOCK_PIN);

#if 0
    gpio_set_function(I2C1_CLOCK_SECONDARY, GPIO_FUNC_I2C);
    gpio_set_function(I2C1_DATA_SECONDARY, GPIO_FUNC_I2C); 
    gpio_pull_up(I2C1_DATA_SECONDARY);
    gpio_pull_up(I2C1_CLOCK_SECONDARY);
#endif

    gpio_init(MUTE_OPTORELAY_PIN);
    gpio_set_dir(MUTE_OPTORELAY_PIN, GPIO_OUT);
    gpio_put(MUTE_OPTORELAY_PIN, LOW);

    gpio_init(PORT_RESET_PIN);
    gpio_set_dir(PORT_RESET_PIN, GPIO_OUT);
    gpio_put(PORT_RESET_PIN, LOW);

#if 0
    /* Reset Ports */
    gpio_put(PORT_RESET_PIN, HIGH);
    sleep_ms(50);
    gpio_put(PORT_RESET_PIN, LOW);
    sleep_ms(50);
#endif

#ifdef DEBUG
    printf("<0> i2c Pins set\n");fflush(stdout);
#endif

    //Core0 core_0(i2c0, EEPROM_ADDR);

    multicore_launch_core1(core_1_entry);

    while(1)
    {
        //gpio_put(PICO_LED, true);
        sleep_ms(1000);
        //gpio_put(PICO_LED, false);
        sleep_ms(1000);

        #if 0
        core_0.startup_routine();
    
        multicore_fifo_push_blocking(0); // syncrhonisation semaphore
        printf("Semaphore Given\n");
        
        core_0.run();
        #endif
    }
}

