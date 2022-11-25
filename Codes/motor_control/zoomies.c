#include "zoomies.h"

const float Kp = 2.5;   
const float Ki = 0.0005;
const float Kd = 0.0025;

uint counterA = 0;
uint counterB = 0;
uint speedA = 0;
uint speedB = 0;
uint pwmA = 7500;
uint pwmB = 7500;

int integralA;
int lastErrorA;
absolute_time_t lastUpdateA;

int integralB;
int lastErrorB;
absolute_time_t lastUpdateB;

void encoderIRQ() {
    //check interrupt flag and status
    if (gpio_get_irq_event_mask(pinENCA) & GPIO_IRQ_EDGE_RISE) {
        //clear interrupt
        gpio_acknowledge_irq(pinENCA, GPIO_IRQ_EDGE_RISE);
        counterA++;
        //printf("===\nCounter A: %d\nCounter B: %d\n", counterA, counterB);
    }    

    if (gpio_get_irq_event_mask(pinENCB) & GPIO_IRQ_EDGE_RISE) {
        gpio_acknowledge_irq(pinENCB, GPIO_IRQ_EDGE_RISE);
        counterB++;
        //printf("===\nCounter A: %d\nCounter B: %d\n", counterA, counterB);
    }  
}

bool calcSpeed(repeating_timer_t* rt) {
    speedA = counterA * (1000/SPEED_CALC_FREQ);
    counterA = 0;
    speedB = counterB * (1000/SPEED_CALC_FREQ);
    counterB = 0;
    // COMMS FUNCTION WILL CALL ON GLOBAL VARIABLES
    printf("===\nCurrent Speed of A: %d notches/s\nCurrent Speed of B: %d notches/s\n", speedA, speedB);
    return true;
}

bool initPWM() {
    //PWM
    //Set pin to pwm function
    gpio_set_function(pinENA, GPIO_FUNC_PWM);
    gpio_set_function(pinENB, GPIO_FUNC_PWM);

    //Set PWM counter to given value. Check docs for slice info
    //Each gpio pin is assigned to a slice number with 2 channels each.
    //each slice uses the same timer, same counter
    //Channel A creates a CCR to do something on when it hits the value assigned to Chan A
    //on below, the counter is set at 1000 for slice 0 (pin 0, 1), Channel A is at a duty cycle of 85% and B is at 65%
    pwm_set_wrap(pwm_gpio_to_slice_num(pinENA), PWM_COUNTER);
    pwm_set_chan_level(pwm_gpio_to_slice_num(pinENA), PWM_CHAN_A, pwmA);
    pwm_set_chan_level(pwm_gpio_to_slice_num(pinENA), PWM_CHAN_B, pwmB);
    pwm_set_enabled(pwm_gpio_to_slice_num(pinENA), true);

    return true;
}

bool computeErrorA() {
    uint currentSpeed = speedA;
    absolute_time_t currentTime = get_absolute_time();
    int64_t deltaTime = absolute_time_diff_us(lastUpdateA, currentTime)/1000;
    int error = TARGET_SPEED - currentSpeed;
    integralA += error * deltaTime;
    int output = (Kp * error) + (Ki * integralA) + (int)(((error - lastErrorA)*Kd)/deltaTime);    


    if ( pwmA + output < 0) {
        pwmA = 0;
    } else {
        if (pwmA + output > PWM_COUNTER) {
            pwmA = PWM_COUNTER;
        } else {
            pwmA += output;
        }
    } 

    printf("A = Integral :%d LastError: %d currentSpeed: %d deltaTime :%"PRId64" Error: %d Output: %d\n", integralA, lastErrorA, currentSpeed, deltaTime, error, pwmA);

    pwm_set_chan_level(pwm_gpio_to_slice_num(pinENA), PWM_CHAN_A, pwmA);

    lastUpdateA = currentTime;
    lastErrorA = error;

    return true;
}

bool computeErrorB() {


    // if (startFlag) {
    //     integralB = 0;
    //     lastErrorB = 0;
    //     lastUpdateB = nil_time;
    // }
    
    uint currentSpeed = speedB;
    absolute_time_t currentTime = get_absolute_time();
    int64_t deltaTime = absolute_time_diff_us(lastUpdateB, currentTime)/1000;
    int error = TARGET_SPEED - currentSpeed;
    integralB += error * deltaTime;
    int output = (Kp * error) + (Ki * integralB) + (int)(((error - lastErrorB)*Kd)/deltaTime);    
    
    if ( pwmB + output < 0) {
        pwmB = 0;
    } else {
        if (pwmB + output > PWM_COUNTER) {
            pwmB = PWM_COUNTER;
        } else {
            pwmB += output;
        }
    } 

    printf("B = Integral :%d LastError: %d currentSpeed: %d deltaTime :%"PRId64" Error: %d Output: %d\n", integralB, lastErrorB, currentSpeed, deltaTime, error, pwmB);

    pwm_set_chan_level(pwm_gpio_to_slice_num(pinENA), PWM_CHAN_B, pwmB);
    lastUpdateB = currentTime;
    lastErrorB = error;

    return true;

}

bool computeError(repeating_timer_t *rt) {
    computeErrorB();
    computeErrorA();

    return true;
}

void resetVariables() {
    integralA = 0;
    lastErrorA = 0;
    lastUpdateA = get_absolute_time();

    integralB = 0;
    lastErrorB = 0;
    lastUpdateB = get_absolute_time();
}

void initWheels(){ 
    //Init GPIO 2 - 5 for wheel direction (pinIN1+pinIN2 for wheel left, pinIN3+pinIN4 for wheel right) 
    gpio_init(pinIN1); 
    gpio_init(pinIN2); 
    gpio_init(pinIN3); 
    gpio_init(pinIN4); 
     
 
    //Set GPIO to out for GPIO 2 - 5 (controls wheel direction) 
    gpio_set_dir(pinIN1, GPIO_OUT); 
    gpio_set_dir(pinIN2, GPIO_OUT); 
    gpio_set_dir(pinIN3, GPIO_OUT); 
    gpio_set_dir(pinIN4, GPIO_OUT); 
 
    //Set GPIO 2 - 5 to LOW 
    gpio_put(pinIN1, 0); 
    gpio_put(pinIN2, 0); 
    gpio_put(pinIN3, 0); 
    gpio_put(pinIN4, 0); 
} 
 
int64_t moveForwards(){ 
    resetVariables();
    gpio_put(pinIN1, 0); 
    gpio_put(pinIN2, 1); 
    gpio_put(pinIN3, 0); 
    gpio_put(pinIN4, 1); 
    return 0;
} 
 
int64_t moveBackwards(int time){
    resetVariables(); 
    gpio_put(pinIN1, 1); 
    gpio_put(pinIN2, 0); 
    gpio_put(pinIN3, 1); 
    gpio_put(pinIN4, 0); 
    add_alarm_in_ms(time, stopMovement,NULL,false);
} 
 
int64_t moveAntiClockWise(int time){ 
    resetVariables();
    gpio_put(pinIN1, 0); 
    gpio_put(pinIN2, 1); 
    gpio_put(pinIN3, 1); 
    gpio_put(pinIN4, 0); 
    add_alarm_in_ms(time, stopMovement,NULL,false);
} 
 
int64_t moveClockWise(int time){ 
    resetVariables();
    gpio_put(pinIN1, 1); 
    gpio_put(pinIN2, 0); 
    gpio_put(pinIN3, 0); 
    gpio_put(pinIN4, 1); 
    add_alarm_in_ms(time, stopMovement,NULL,false);
}

int64_t stopMovement(alarm_id_t id, void *user_data) {
    resetVariables();
    gpio_put(pinIN1, 0); 
    gpio_put(pinIN2, 0); 
    gpio_put(pinIN3, 0); 
    gpio_put(pinIN4, 0); 
}