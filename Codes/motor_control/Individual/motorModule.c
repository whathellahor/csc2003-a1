#include "motorModule.h"

const float Kp = 8.0;   
const float Ki = 0.001;
const float Kd = 5.0;

uint counterA = 0;
uint counterB = 0;
uint speedA = 0;
uint speedB = 0;
uint pwmA = PWM_DEFAULTS;
uint pwmB = PWM_DEFAULTS;

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

    //Slot in accelerometer computation
    //triggerAcc_callback();
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
    //on below, the counter is set at 10000 for slice 0 (pin 0, 1), Channel A is at a duty cycle of 85% and B is at 65%
    pwm_set_wrap(pwm_gpio_to_slice_num(pinENA), PWM_COUNTER);
    pwm_set_chan_level(pwm_gpio_to_slice_num(pinENA), PWM_CHAN_A, pwmA);
    pwm_set_chan_level(pwm_gpio_to_slice_num(pinENA), PWM_CHAN_B, pwmB);
    pwm_set_enabled(pwm_gpio_to_slice_num(pinENA), true);

    return true;
}

bool computeErrorA() {
    uint currentSpeed = speedA;
    //Computes timeDelta for integral and derivatives
    absolute_time_t currentTime = get_absolute_time();
    int64_t deltaTime = absolute_time_diff_us(lastUpdateA, currentTime)/1000;
    //Compute errors, integral
    int error = TARGET_SPEED - currentSpeed;
    integralA += error * deltaTime;
    //Compute PWM correction and derivative
    int output = (Kp * error) + (Ki * integralA) + (int)(((error - lastErrorA)*Kd)/deltaTime);    

    //Limit corrected PWM pulse to range 0-10000
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
    //Set new PWM on ENA pin
    pwm_set_chan_level(pwm_gpio_to_slice_num(pinENA), PWM_CHAN_A, pwmA);

    //Stores data for next cycle
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
    //Computes timeDelta for integral and derivatives
    absolute_time_t currentTime = get_absolute_time();
    int64_t deltaTime = absolute_time_diff_us(lastUpdateB, currentTime)/1000;
    //Compute errors, integral
    int error = TARGET_SPEED - currentSpeed;
    integralB += error * deltaTime;
    //Compute PWM correction and derivative
    int output = (Kp * error) + (Ki * integralB) + (int)(((error - lastErrorB)*Kd)/deltaTime);    
    
    //Limit corrected PWM pulse to range 0-10000
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
    //Set new PWM on ENA pin
    pwm_set_chan_level(pwm_gpio_to_slice_num(pinENA), PWM_CHAN_B, pwmB);
    //Stores data for next cycle
    lastUpdateB = currentTime;
    lastErrorB = error;

    return true;

}

//Shorthand to call individual PID function, used by timer
bool computeError(repeating_timer_t *rt) {
    computeErrorB();
    computeErrorA();
    return true;
}

//Resets PID input for new movement
void resetVariables() {
    integralA = 0;
    lastErrorA = 0;
    lastUpdateA = get_absolute_time();
    pwmA = PWM_DEFAULTS;

    integralB = 0;
    lastErrorB = 0;
    lastUpdateB = get_absolute_time();
    pwmB = PWM_DEFAULTS;
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
 
void moveForwards(int time){ 
    resetVariables();
    gpio_put(pinIN1, 0); 
    gpio_put(pinIN2, 1); 
    gpio_put(pinIN3, 0); 
    gpio_put(pinIN4, 1); 
    add_alarm_in_ms(time, stopMovement,NULL,false);
} 
 
void moveBackwards(int time){
    resetVariables(); 
    gpio_put(pinIN1, 1); 
    gpio_put(pinIN2, 0); 
    gpio_put(pinIN3, 1); 
    gpio_put(pinIN4, 0); 
    add_alarm_in_ms(time, stopMovement,NULL,false);
}
 
void moveAntiClockWise(int time){ 
    resetVariables();
    gpio_put(pinIN1, 1); 
    gpio_put(pinIN2, 0); 
    gpio_put(pinIN3, 0); 
    gpio_put(pinIN4, 1); 
    add_alarm_in_ms(time, stopMovement,NULL,false);
} 
 
void moveClockWise(int time){ 
    resetVariables();
    gpio_put(pinIN1, 0); 
    gpio_put(pinIN2, 1); 
    gpio_put(pinIN3, 1); 
    gpio_put(pinIN4, 0); 
    add_alarm_in_ms(time, stopMovement,NULL,false);
}

int64_t stopMovement(alarm_id_t id, void *user_data) {
    resetVariables();
    gpio_put(pinIN1, 0); 
    gpio_put(pinIN2, 0); 
    gpio_put(pinIN3, 0); 
    gpio_put(pinIN4, 0); 
    return 0; //Do not repeat
}

void forward() {
    moveForwards(20000);
    //moveForwards(1200);
}

void rightTurn() {
    moveClockWise(500);
}

void leftTurn() {
    moveAntiClockWise(420);
}

uint getAvgSpeed() {
    return (speedA + speedB)/2;
}

int main() {
    //Enable serial output
    stdio_init_all(); 
    //Wait for Init
    sleep_ms(10000);  
    //Init Encoders
    initWheels();

    repeating_timer_t speedTimer;
    repeating_timer_t pidTimer;

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

    gpio_set_irq_enabled(pinENCA, GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(pinENCB, GPIO_IRQ_EDGE_RISE, true);

    //Define interrupt handler function
    gpio_add_raw_irq_handler_masked(pinENCA | pinENB , encoderIRQ);

    //Enable interrupt on GPIO
    irq_set_enabled(IO_IRQ_BANK0, true);

    forward();
    while (1) {
        tight_loop_contents();
    }
}