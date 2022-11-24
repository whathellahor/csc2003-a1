// include libraries
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include <stdio.h>
#include <math.h>
//#include "ultrasonic.h"

// global variables
#define MIN_DISTANCE 15 // distance 15cm, car stop
#define MIN_TRIGGER_TIME 10 // min time pulse high (for 10 microseconds)
#define MAX_UPTIME 3 // time to timeout
// testing
#define LED_PIN 25
// ultrasonic for right side of car
#define RIGHT_TRIGGER_PIN 10 // output pin
#define RIGHT_ECHO_PIN 11 // input pin
// ultrasonic for left side of car
#define LEFT_TRIGGER_PIN 12
#define LEFT_ECHO_PIN 13
// ultrasonic for center of car
#define CENTER_TRIGGER_PIN 14
#define CENTER_ECHO_PIN 15
// shoots pulse every 30 microseconds
#define PULSE_INTERVAL 30
// smoothing sample size
#define SAMPLE_SIZE 30


// IRQ handlers
int64_t triggerAlarm_callback(alarm_id_t id, void *user_data); // shooting pulse IRQ
int64_t EchoMaxAlarm_callback_center(alarm_id_t id, void *user_data); // center car IRQ
int64_t EchoMaxAlarm_callback_left(alarm_id_t id, void *user_data); // left side of car IRQ
int64_t EchoMaxAlarm_callback_right(alarm_id_t id, void *user_data); // right side of car IRQ

// struct function for sensors
struct Sensors{
    uint8_t triggerPin;
    uint8_t echoPin;
    uint8_t distance;
    absolute_time_t startTime;
    absolute_time_t endTime;
    bool timeOut; // timeout to stop calculating distance after a certain time (force falling edge)
    // smoothing algorithm
    uint8_t averageDistance;
    uint8_t distanceArray[SAMPLE_SIZE]; // array to store "sample size" number of readings
    uint8_t numberofElements;
    uint8_t noOfElements;
    // print name of sensor (left, center, right)
    char Name;
} sensor_center, sensor_left, sensor_right; // 3 sensors: center ultrasound, left ultrasound, right ultrasound


//initialize all sensors struct values
void initializeSensorValues()
{
    // center ultrasonic
    sensor_center.triggerPin = CENTER_TRIGGER_PIN;
    sensor_center.echoPin = CENTER_ECHO_PIN;
    // left ultrasonic
    sensor_left.triggerPin = LEFT_TRIGGER_PIN;
    sensor_left.echoPin = LEFT_ECHO_PIN;
    // right ultrasonic
    sensor_right.triggerPin = RIGHT_TRIGGER_PIN;
    sensor_right.echoPin = RIGHT_ECHO_PIN;
    // name of ultrasonic
    sensor_center.Name = 'C';
    sensor_left.Name = 'L';
    sensor_right.Name = 'R';

    // intialise sensors distancem average distance = 0
    sensor_center.distance = sensor_left.distance = sensor_right.distance = 0;
    sensor_center.averageDistance = sensor_left.averageDistance = sensor_right.averageDistance = 0;
    // smoothing algorithm
    // initialise noOfElements in array = 0 []
    sensor_center.noOfElements= sensor_left.noOfElements = sensor_right.noOfElements = 0;
    // boolean to check sensor timeOut
    sensor_center.timeOut = sensor_left.timeOut = sensor_right.timeOut = false;
}


// setup pins
void setupUltrasonicPins()
{
    // initialise center ultrasonic pins
    gpio_init(CENTER_TRIGGER_PIN);
    gpio_init(CENTER_ECHO_PIN);
    // initialise left ultrasonic pins
    gpio_init(LEFT_TRIGGER_PIN);
    gpio_init(LEFT_ECHO_PIN);
    // initialise right ultrasonic pins
    gpio_init(RIGHT_TRIGGER_PIN);
    gpio_init(RIGHT_ECHO_PIN);

    // initialise led pin (for testing)
    gpio_init(LED_PIN);

    // setup gpio direction

    // center ultrasonic pins
    gpio_set_dir(CENTER_TRIGGER_PIN, GPIO_OUT);
    gpio_set_dir(CENTER_ECHO_PIN, GPIO_IN);
    // left ultrasonic pins
    gpio_set_dir(LEFT_TRIGGER_PIN, GPIO_OUT);
    gpio_set_dir(LEFT_ECHO_PIN, GPIO_IN);
    // right ultrasonic pins
    gpio_set_dir(RIGHT_TRIGGER_PIN, GPIO_OUT);
    gpio_set_dir(RIGHT_ECHO_PIN, GPIO_IN);

    // testing led pin
    gpio_set_dir(LED_PIN, GPIO_OUT);

}

// smoothing algorithm
void Smoothing(struct Sensors *sensor)
{
    // if no of elements in array is less than sample size
    // check if array is lesser than sample size
    // adds distance into array until noOfElements = sample size
    if(sensor -> noOfElements < SAMPLE_SIZE)
    {
        // sensor points to distance array
        // sensor points and reads how many elements are there in array
        // sensor points to distance [ _distance1_, _distance2_, ... ]
        sensor -> distanceArray[ sensor -> noOfElements ] = sensor -> distance;
        sensor -> noOfElements++;
    }
    else
    {  
        // initialise arraySum (add distance values in array together)
        int arraySum = 0;
        // shift elements in array to the left
        for( int i = 0; i < SAMPLE_SIZE - 1; i++ )
        {
            sensor -> distanceArray[ i ] = sensor -> distanceArray[ i + 1 ];
        }
        // append new distance to the end of the array (last element of array)
        sensor -> distanceArray[ SAMPLE_SIZE - 1 ] = sensor -> distance;

        // calculate average
        // sum of total distance (in array) / no of samples (sample size)

        // calculate total distance inside array (add up all the distance values in array)
        for(int i = 0; i < SAMPLE_SIZE; i++)
        {
            arraySum += sensor -> distanceArray[ i ];
        }

        // calculate average distance (total distance / sample size)
        sensor -> averageDistance = arraySum / SAMPLE_SIZE;

        // print output in putty
        printf("%c -> distance: %d cm, average: %d cm \n", sensor -> Name, sensor -> distance, sensor -> averageDistance);
    }
}

// set the trigger pin to high
void setTriggerPinHigh(uint trigPin)
{
    gpio_put(trigPin, 1);
}

// set trigger pin low
void setTriggerPinLow(uint trigPin)
{
    gpio_put(trigPin, 0);
}

// set the echo pin High
void setEchoPinHigh(uint echoPin)
{
    gpio_put(echoPin, 1);
}

// set the echo pin low
void setEchoPinLow(uint echoPin)
{
    gpio_put(echoPin, 0);
}

// calculate distance
void getDistance(uint timeDifference, struct Sensors *sensor)
{
    uint8_t pulseLength = timeDifference / 58;
    sensor -> distance = pulseLength;
    // use smoothing algorithm to callibrate distance (using running average)
    Smoothing(sensor);
}


// callback function for rising / falling edge IRQ for center sensor
void echoUltraSonic_callback_center()
{
    // if rising edge detected
    if(gpio_get_irq_event_mask(sensor_center.echoPin) & GPIO_IRQ_EDGE_RISE)
    {
        // acknowledge rising edge
        gpio_acknowledge_irq(sensor_center.echoPin, GPIO_IRQ_EDGE_RISE);
        // get start time at rising edge
        sensor_center.startTime = get_absolute_time();
        // start timer for timeout (timeout detect rising edge at certain time, force falling edge)
        add_alarm_in_ms(MAX_UPTIME, EchoMaxAlarm_callback_center, NULL, true);
    }
    // if falling edge detected
    else if(gpio_get_irq_event_mask(sensor_center.echoPin) & GPIO_IRQ_EDGE_FALL)
    {
        // acknowledge falling edge
        gpio_acknowledge_irq(sensor_center.echoPin, GPIO_IRQ_EDGE_FALL);
        // if timeOut true (means falling edge was forced)
        if (sensor_center.timeOut == true)
        {
            // reset timeOut
            sensor_center.timeOut = false;
            // reset distance (start reading another distance)
            sensor_center.averageDistance = 0;
            return;
        }

        // get end time at falling edge
        sensor_center.endTime = get_absolute_time();
        // get time difference (end time - start time)
        uint timeDifference = absolute_time_diff_us(sensor_center.startTime, sensor_center.endTime);
        // get pulse width based on center ultrasonic sensor input
        getDistance(timeDifference, &sensor_center);
    }
}

// callback function for rising / falling edge IRQ for left sensor
void echoUltraSonic_callback_left()
{
    if(gpio_get_irq_event_mask(sensor_left.echoPin) & GPIO_IRQ_EDGE_RISE)
    {
        gpio_acknowledge_irq(sensor_left.echoPin, GPIO_IRQ_EDGE_RISE);
        sensor_left.startTime = get_absolute_time();
        add_alarm_in_ms(MAX_UPTIME, EchoMaxAlarm_callback_left, NULL, true);
    }
    else if(gpio_get_irq_event_mask(sensor_left.echoPin) & GPIO_IRQ_EDGE_FALL)
    {
        gpio_acknowledge_irq(sensor_left.echoPin, GPIO_IRQ_EDGE_FALL);
        if (sensor_left.timeOut == true)
        {
            sensor_left.timeOut = false;
            sensor_left.averageDistance = 0;
            return;
        }

        sensor_left.endTime = get_absolute_time();
        uint timeDifference = absolute_time_diff_us(sensor_left.startTime, sensor_left.endTime);
        getDistance(timeDifference, &sensor_left);
    }
}

// callback function for rising / falling edge IRQ for right sensor
void echoUltraSonic_callback_right()
{
   if(gpio_get_irq_event_mask(sensor_right.echoPin) & GPIO_IRQ_EDGE_RISE)
    {
        gpio_acknowledge_irq(sensor_right.echoPin, GPIO_IRQ_EDGE_RISE);
        sensor_right.startTime = get_absolute_time();
        add_alarm_in_ms(MAX_UPTIME, EchoMaxAlarm_callback_right, NULL, true);
    }
    else if(gpio_get_irq_event_mask(sensor_right.echoPin) & GPIO_IRQ_EDGE_FALL)
    {
        gpio_acknowledge_irq(sensor_right.echoPin, GPIO_IRQ_EDGE_FALL);
        if (sensor_right.timeOut == true)
        {
            sensor_right.averageDistance = 0 ;
            sensor_right.timeOut = false;
            return;
        }

        sensor_right.endTime = get_absolute_time();
        uint timeDifference = absolute_time_diff_us(sensor_right.startTime, sensor_right.endTime);
        getDistance(timeDifference, &sensor_right);
    }
}


// set the trigger pin to high and set alarm to off after 10 microseconds
bool triggerUltraSonic_callback(struct repeating_timer *t)
{
        setTriggerPinHigh(sensor_center.triggerPin);
        setTriggerPinHigh(sensor_left.triggerPin);
        setTriggerPinHigh(sensor_right.triggerPin);
        add_alarm_in_us(MIN_TRIGGER_TIME, triggerAlarm_callback, NULL, true);
        return true;   
}

// callback function to set trigger low
int64_t triggerAlarm_callback(alarm_id_t id, void *user_data)
{
    setTriggerPinLow(sensor_center.triggerPin);
    setTriggerPinLow(sensor_left.triggerPin);
    setTriggerPinLow(sensor_right.triggerPin);
    alarm_pool_cancel_alarm(alarm_pool_get_default(), id);
    return 10;
}


// callback function to stop calculating distance if no echo is detected
int64_t EchoMaxAlarm_callback_center(alarm_id_t id, void *user_data)
{
    if (gpio_get(sensor_center.echoPin) == 1 && sensor_center.timeOut == false)
    {
        sensor_center.timeOut = true;
        setEchoPinLow(sensor_center.echoPin);
        sensor_center.distance = 0;
       
    }
    alarm_pool_cancel_alarm(alarm_pool_get_default(), id);
    return 0;
}

// callback alarm for left sensor
int64_t EchoMaxAlarm_callback_left(alarm_id_t id, void *user_data)
{
    if (gpio_get(sensor_left.echoPin) == 1 && sensor_left.timeOut == false)
    {
        sensor_left.timeOut = true;
        setEchoPinLow(sensor_left.echoPin);
        sensor_left.distance=  0;
    }
    alarm_pool_cancel_alarm(alarm_pool_get_default(), id);
    return 0;
}
// callback alarm for right sensor
int64_t EchoMaxAlarm_callback_right(alarm_id_t id, void *user_data)
{
    if (gpio_get(sensor_right.echoPin) == 1 && sensor_right.timeOut == false)
    {
        sensor_right.timeOut = true;
        setEchoPinLow(sensor_right.echoPin);
        sensor_right.distance = 0;
    }
    alarm_pool_cancel_alarm(alarm_pool_get_default(), id);
    return 0;
}

int main()
{
    stdio_init_all();

    alarm_pool_init_default();

    // setup struct values
    initializeSensorValues();

    // setup ultrasonic pins
    setupUltrasonicPins();

    // set up the repeating timer to trigger the ultrasonic sensor
    struct repeating_timer triggerTimer;
    add_repeating_timer_ms(PULSE_INTERVAL, triggerUltraSonic_callback, NULL, &triggerTimer);

    gpio_set_irq_enabled(sensor_center.echoPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(sensor_left.echoPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(sensor_right.echoPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

    // set up the callback function for the echo pin(s)
    gpio_add_raw_irq_handler(sensor_center.echoPin, echoUltraSonic_callback_center);
    gpio_add_raw_irq_handler(sensor_left.echoPin, echoUltraSonic_callback_left);
    gpio_add_raw_irq_handler(sensor_right.echoPin, echoUltraSonic_callback_right);

    // enable interrupt master
    irq_set_enabled(IO_IRQ_BANK0,true);

   
    while (true)
    {
        tight_loop_contents();
        if (sensor_center.averageDistance <= MIN_DISTANCE && sensor_center.averageDistance > 0  && sensor_center.timeOut == false)
        {
            gpio_put(LED_PIN, 1);
             
        }
        else
        {
            gpio_put(LED_PIN, 0);
        }
    }
}