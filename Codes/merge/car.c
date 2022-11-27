#include "../ultrasonic_accelerometer/accelerometer/accelerometer.h"
#include "../motor_control/zoomies.h"
#include "../ultrasonic_accelerometer/ultrasonic/ultrasonic.h"
#include "../infrared/barcode/barcode.h"
#include "../communicatations/esp-uart/comm_mod.h"
#include "pico/time.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"

/*FUNCTION PROTOTYPE*/
void update_car();

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

/***************************************************************************
 * UPDATE COMMS GLOBAL VARIABLE, car_data
 * TO BE PARKED IN SOME TIMER/ INTERRUPT, AFTER ALL MODS INCLUDE THEIR DATA
 * TO BE OUTPUTTED TO WEBAPP
****************************************************************************/
void update_car() {
    car_data.hump = NULL; // TOTAL NUMBER OF HUMPS, UINT8_T
    car_data.turn = NULL ; // TOTAL NUMBER OF TURNS, UINT8_T
    car_data.hump_detected = NULL; // 1 FOR DETECTED, 0 FOR NOT DETECTED, UINT8_T
    car_data.turn_detected = NULL; // 1 FOR DETECTED, 0 FOR NOT DETECTED, UINT8_T
    car_data.distance = NULL; // TOTAL DISTANCE TRAVELLED, UINT8_T
    car_data.speed = NULL; // CURRENT SPEED, UINT8_T
    sprintf(car_data.barcode, "%s", NULL); // ACCEPT STRING UP TO LEN 99, OR ANY OTHER FORMAT JUST CHANGE THE SPECIFIER
    sprintf(car_data.map_data, "%s", NULL); // ACCEPT STRING UP TO LEN 99, OR ANY OTHER FORMAT JUST CHANGE THE SPECIFIER
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
        forward();
        sleep_ms(2000);
        // rightTurn();
        // sleep_ms(2000);
        // leftTurn();
        // sleep_ms(2000);
        //tight_loop_contents();
        // call main function to start mapping
        //startMapping();

        //call main function to start navigating
        //startNavigating();

        /*************************************************************
         * START COMMS
         * REPLACE SSID AND PASS WITH OWN TO TEST
         * DO IP SCAN TO ACQUIRE IP IF NO DEBUG OUTPUT
         * CONNECTING TO PICO IP ON BROWSER WILL DISPLAY JSON VALUES
         * init_comms() IS A BLOCKING FUNCTION
        *************************************************************/
        char ssid[] = "pico_test1"; // REPLACE WITH OWN SSID
        char pass[] = "testtest"; // REPLACE WITH OWN PASSWORD
        init_comms(1, ssid, pass); // 1: STATION MODE, 2: HOST, 3: BOTH
        /*************************************************************
         * ANYTHING FROM HERE ONWARDS SHOULDN'T RUN
         * END COMMS
        **************************************************************/

    }

}