#include "../ultrasonic_accelerometer/accelerometer/accelerometer.h"
#include "../motor_control/zoomies.h"
#include "../ultrasonic_accelerometer/ultrasonic/ultrasonic.h"
#include "../infrared/barcode/barcode.h"
#include "pico/time.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"


void IRQ_HANDLER() {
    //Checks interrupt source and handle appropriately
    if ( gpio_get_irq_event_mask(pinENCA) & GPIO_IRQ_EDGE_RISE ||  gpio_get_irq_event_mask(pinENCB) & GPIO_IRQ_EDGE_RISE) {
        encoderIRQ();
    } else {
        echoUltraSonic_callback();
    }
}

repeating_timer_t speedTimer;
repeating_timer_t pidTimer;
repeating_timer_t triggerTimer;

void barcodeCore(){
    
    // initBarcode(speedA,speedB);
    initBarcode();
}

int main() {
    //Enable serial output
    stdio_init_all(); 
    //Wait for Init
    sleep_ms(10000);  
    //Init Accelerometer over I2C
    initAcc();      
    //Init Barcode polling on core1  
    multicore_launch_core1(barcodeCore); 
    //Init Alarm Pool
    alarm_pool_init_default(); 
    
    //Init Ultrasonic Sensors
    initializeSensorValues();
    setupUltrasonicPins();

    //Init Encoders
    initWheels();

    //Speed Calc
    //create a timer to call a given function (i.e. calcSpeed) every n milliseconds) returns false if no timer slots available
    if (!add_repeating_timer_ms(SPEED_CALC_FREQ, calcSpeed, NULL, &speedTimer)) {
         return 1;
    };

    //Init and Start PWM on motors
    initPWM();

    //PID Controller Loop
    //Add timer to run PID periodically
    if (!add_repeating_timer_ms(PID_FREQ, computeError, NULL, &pidTimer)) {
         return 1;
    };

    //Adds a timer to send pulses from ultrasonic sensors
    if (!add_repeating_timer_ms(PULSE_INTERVAL, triggerUltraSonic_callback , NULL, &triggerTimer)){
        return 1;  
    };

    //Set up GPIO interrupt on Ultrasonic sensor and Encoder pins
    gpio_set_irq_enabled(sensor_center.echoPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(sensor_left.echoPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(sensor_right.echoPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(pinENCA, GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(pinENCB, GPIO_IRQ_EDGE_RISE, true);

    //Define interrupt handler function
    gpio_add_raw_irq_handler_masked( sensor_center.echoPin | sensor_left.echoPin | sensor_right.echoPin | pinENCA | pinENB , IRQ_HANDLER);

    //Enable interrupt on GPIO
    irq_set_enabled(IO_IRQ_BANK0, true);

    while (1) {
        // forward();
        // sleep_ms(6000);
        // rightTurn();
        // sleep_ms(2000);
        // leftTurn();
        // sleep_ms(2000);
        tight_loop_contents();
        // call main function to start mapping
        //startMapping();

        //call main function to start navigating
        //startNavigating();
    }

}