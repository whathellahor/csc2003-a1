#include "../motor_control/zoomies.h"
#include "../ultrasonic_accelerometer/ultrasonic/ultrasonic.h"

int main() {
    repeating_timer_t timer;
    repeating_timer_t timer2;
    struct repeating_timer triggerTimer;

    stdio_init_all();  
    //enable uart output with stdlib
    sleep_ms(1000);
    printf("Initialise Zoom");

    alarm_pool_init_default();
    // setup struct values
    initializeSensorValues();
    // setup ultrasonic pins
    setupUltrasonicPins();

    if (!add_repeating_timer_ms(PULSE_INTERVAL, triggerUltraSonic_callback, NULL, &triggerTimer)) {
        printf("Failed to add timer\n");
        return 1; //Failed to add timer, terminate
    }

    gpio_set_irq_enabled(sensor_center.echoPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(sensor_left.echoPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(sensor_right.echoPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_add_raw_irq_handler(sensor_center.echoPin, echoUltraSonic_callback_center);
    gpio_add_raw_irq_handler(sensor_left.echoPin, echoUltraSonic_callback_left);
    gpio_add_raw_irq_handler(sensor_right.echoPin, echoUltraSonic_callback_right);

    initWheels();

    if (!initEncoders(&timer)){
        printf("Failed to add timer\n");
        return 1; //Failed to add timer, terminate
    };

    if (!initPWM(&timer2)) {
        printf("Failed to add timer\n");
        return 1; //Failed to add timer, terminate
    }
    
    //Enable interrupt on GPIO
    irq_set_enabled(IO_IRQ_BANK0, true);

    while (1)
        tight_loop_contents();

}