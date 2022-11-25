#include "../motor_control/zoomies.h"
#include "../ultrasonic_accelerometer/ultrasonic/ultrasonic.h"
#include "pico/time.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"

void IRQ_HANDLER() {
    //if (gpio_get_irq_event_mask(sensor_center.echoPin) & (GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL) || gpio_get_irq_event_mask(sensor_right.echoPin) & (GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL) || gpio_get_irq_event_mask(sensor_left.echoPin) & (GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL)) {
    if ( gpio_get_irq_event_mask(pinENCA) & GPIO_IRQ_EDGE_RISE ||  gpio_get_irq_event_mask(pinENCB) & GPIO_IRQ_EDGE_RISE) {
        encoderIRQ();
    } else {
        //encoderIRQ();
        echoUltraSonic_callback();
    }
}

void TimerTest() {
    printf("Hello\n");
}

repeating_timer_t timer;
repeating_timer_t timer2;
repeating_timer_t triggerTimer;

int main() {


    stdio_init_all();  
    //enable uart output with stdlib
    sleep_ms(10000);
    printf("Initialise Zoom\n");

    alarm_pool_init_default();
    printf("Alarm pool: %d\n", alarm_pool_hardware_alarm_num(alarm_pool_get_default()));
    // setup struct values
    initializeSensorValues();
    printf("1\n");
    // setup ultrasonic pins
    setupUltrasonicPins();
    printf("1\n");
    initWheels();
    printf("1\n");
    //Speed Calc
    //create a timer to call a given function (i.e. calcSpeed) every n milliseconds) returns false if no timer slots available
    if (!add_repeating_timer_ms(SPEED_CALC_FREQ, calcSpeed, NULL, &timer)) {
         printf("Failed to add timer\n");
         return 1;
    };
    printf("Succesfully added enc timer\n");

    initPWM();
    //Add timer to run PID periodically
    if (!add_repeating_timer_ms(PID_FREQ, computeError, NULL, &timer2)) {
         printf("Failed to add timer\n");
         return 1;
    };
    printf("Succesfully added pwm timer\n");

    if (!add_repeating_timer_ms(PULSE_INTERVAL, triggerUltraSonic_callback , NULL, &triggerTimer)){
        printf("Failed to add timer\n");
        return 1;  
    };
    printf("1\n");

    gpio_set_irq_enabled(sensor_center.echoPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(sensor_left.echoPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(sensor_right.echoPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(pinENCA, GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(pinENCB, GPIO_IRQ_EDGE_RISE, true);
    printf("all gpio irq setup\n");

    gpio_add_raw_irq_handler_masked( sensor_center.echoPin | sensor_left.echoPin | sensor_right.echoPin | pinENCA | pinENB , IRQ_HANDLER);
    printf("Interrupt handler added\n");
    printf("all gpio irq assigned\n");

    //Enable interrupt on GPIO
    irq_set_enabled(IO_IRQ_BANK0, true);
    printf("all irq enabled\n");

    moveForwards();

    while (1)
        tight_loop_contents();

}