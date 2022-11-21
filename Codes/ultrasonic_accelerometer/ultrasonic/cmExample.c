#include "pico/stdlib.h"
#include <stdio.h>
//#include "ultrasonic.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include <math.h>

#define MIN_DISTANCE 15
#define MIN_TRIGGER_TIME 10
#define MAX_UPTIME 3
#define LED_PIN 25
#define RIGHT_TRIGGER_PIN 10
#define RIGHT_ECHO_PIN 11
#define LEFT_TRIGGER_PIN 12
#define LEFT_ECHO_PIN 13
#define CENTER_TRIGGER_PIN 14
#define CENTER_ECHO_PIN 15

//uint trigPin = 14;
//uint echoPin = 15;
int64_t triggerAlarm_callback(alarm_id_t id, void *user_data);
int64_t EchoMaxAlarm_callback(alarm_id_t id, void *user_data);
int64_t EchoMaxAlarm_callback_center(alarm_id_t id, void *user_data);
int64_t EchoMaxAlarm_callback_left(alarm_id_t id, void *user_data);
int64_t EchoMaxAlarm_callback_right(alarm_id_t id, void *user_data);

uint sampleSize = 30;
uint totalReadings = 2000;


struct Sensors{
    uint8_t triggerPin;
    uint8_t echoPin;
    uint8_t distance;
    absolute_time_t startTime;
    absolute_time_t endTime;
    uint8_t averageDistance;
    uint8_t distanceArray[40];
    uint8_t numberofElements;
    uint8_t noOfElements;
    bool timeOut;
}sensor_center, sensor_left, sensor_right;


//initialize all sensors struct values
void initializeSensorValues()
{
    sensor_center.triggerPin = CENTER_TRIGGER_PIN;
    sensor_center.echoPin = CENTER_ECHO_PIN;
    sensor_left.triggerPin = LEFT_TRIGGER_PIN;
    sensor_left.echoPin = LEFT_ECHO_PIN;
    sensor_right.triggerPin = RIGHT_TRIGGER_PIN;
    sensor_right.echoPin = RIGHT_ECHO_PIN;

    sensor_center.distance = sensor_left.distance = sensor_right.distance = 0;
    sensor_center.averageDistance = sensor_left.averageDistance = sensor_right.averageDistance = 0;
    sensor_center.noOfElements= sensor_left.noOfElements = sensor_right.noOfElements = 0;
    sensor_center.timeOut = sensor_left.timeOut = sensor_right.timeOut = false;
}


// Setup Pins
void setupUltrasonicPins()
{
    gpio_init(CENTER_TRIGGER_PIN);
    gpio_init(CENTER_ECHO_PIN);

    // gpio_init(LEFT_TRIGGER_PIN);
    // gpio_init(LEFT_ECHO_PIN);

    // gpio_init(RIGHT_TRIGGER_PIN);
    // gpio_init(RIGHT_ECHO_PIN);

    gpio_init(LED_PIN);

    gpio_set_dir(CENTER_TRIGGER_PIN, GPIO_OUT);
    gpio_set_dir(CENTER_ECHO_PIN, GPIO_IN);

    // gpio_set_dir(LEFT_TRIGGER_PIN, GPIO_OUT);
    // gpio_set_dir(LEFT_ECHO_PIN, GPIO_IN);

    // gpio_set_dir(RIGHT_TRIGGER_PIN, GPIO_OUT);
    // gpio_set_dir(RIGHT_ECHO_PIN, GPIO_IN);

    gpio_set_dir(LED_PIN, GPIO_OUT);

}

void Smoothing(struct Sensors *sensor)
{
    if(sensor->noOfElements<sampleSize)
    {
        sensor->distanceArray[sensor->noOfElements] = sensor->distance;
        sensor->distanceArray[sensor->noOfElements+1] = sensor->distance;
        sensor->noOfElements++;
    }
    else
    {  
        int arraySum = 0;
        //Shift elements in array to the left
        for(int i=0; i<sampleSize-1; i++)
        {
            sensor->distanceArray[i] = sensor->distanceArray[i+1];
        }
        //Add new distance to the end of the array
        sensor->distanceArray[sampleSize-1] = sensor->distance;

        //Calculate average
        for(int i=0; i<sampleSize; i++)
        {
            arraySum += sensor->distanceArray[i];
        }

        sensor->averageDistance = arraySum/sampleSize;
        printf("distance: %d cm, average: %d cm \n", sensor->distance, sensor->averageDistance);
    }
}

// set the trigger pin to high
void setTriggerPinHigh(uint trigPin)
{
    gpio_put(trigPin, 1);
}

// Set trigger pin low
void setTriggerPinLow(uint trigPin)
{
    gpio_put(trigPin, 0);
}

// Set the echo pin High
void setEchoPinHigh(uint echoPin)
{
    gpio_put(echoPin, 1);
}

// Set the echo pin low
void setEchoPinLow(uint echoPin)
{
    gpio_put(echoPin, 0);
}

// Calculate Distance
void getDistance(uint timeDifference,struct Sensors *sensor)
{
    uint8_t pulseLength = timeDifference / 58;
    sensor->distance = pulseLength;
   // printf("distance: %d cm", sensor->distance);
    Smoothing(sensor);
}


// Callback function for Rising|Falling edge IRQ for center sensor
void echoUltraSonic_callback_center()
{
    if(gpio_get_irq_event_mask(sensor_center.echoPin) & GPIO_IRQ_EDGE_RISE)
    {
        gpio_acknowledge_irq(sensor_center.echoPin, GPIO_IRQ_EDGE_RISE);
        sensor_center.startTime = get_absolute_time();
        add_alarm_in_ms(MAX_UPTIME, EchoMaxAlarm_callback_center, NULL, true);
    }
    else if(gpio_get_irq_event_mask(sensor_center.echoPin) & GPIO_IRQ_EDGE_FALL)
    {
        gpio_acknowledge_irq(sensor_center.echoPin, GPIO_IRQ_EDGE_FALL);
        if (sensor_center.timeOut == true)
        {
            sensor_center.timeOut = false;
            return;
        }

        sensor_center.endTime = get_absolute_time();
        uint timeDifference = absolute_time_diff_us(sensor_center.startTime, sensor_center.endTime);
        getDistance(timeDifference, &sensor_center);
    }
}

// Callback function for Rising|Falling edge IRQ for left sensor
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
            return;
        }

        sensor_left.endTime = get_absolute_time();
        uint timeDifference = absolute_time_diff_us(sensor_left.startTime, sensor_left.endTime);
        getDistance(timeDifference, &sensor_left);
    }
}

// Callback function for Rising|Falling edge IRQ for right sensor
void echoUltraSonic_callback_right(uint8_t echoPin, uint32_t events)
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
            sensor_right.timeOut = false;
            return;
        }

        sensor_right.endTime = get_absolute_time();
        uint timeDifference = absolute_time_diff_us(sensor_right.startTime, sensor_right.endTime);
        getDistance(timeDifference, &sensor_right);
    }
}


// Set the trigger pin to high and set alarm to off after 10 microseconds
bool triggerUltraSonic_callback(struct repeating_timer *t)
{
        setTriggerPinHigh(sensor_center.triggerPin);
        // setTriggerPinHigh(sensor_left.triggerPin);
        // setTriggerPinHigh(sensor_right.triggerPin);
        //triggerAlarm = add_alarm_in_us(MIN_TRIGGER_TIME, triggerAlarm_callback, NULL, true);
        add_alarm_in_us(MIN_TRIGGER_TIME, triggerAlarm_callback, NULL, true);
        // totalReadings--;
        return true;   
}

// Callback function to set trigger low
int64_t triggerAlarm_callback(alarm_id_t id, void *user_data)
{
    setTriggerPinLow(sensor_center.triggerPin);
    // setTriggerPinLow(sensor_left.triggerPin);
    // setTriggerPinLow(sensor_right.triggerPin);
    alarm_pool_cancel_alarm(alarm_pool_get_default(), id);
    return 10;
}


// Callback function to Stop calculating distance if no echo is detected
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

//callback alarm for left sensor
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
//callback alarm for right sensor
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

    //Setup struct values
    initializeSensorValues();

    // Setup Ultrasonic Pins
    setupUltrasonicPins();

    // Set up the repeating timer to trigger the ultrasonic sensor
    struct repeating_timer triggerTimer;
    add_repeating_timer_ms(30, triggerUltraSonic_callback, NULL, &triggerTimer);

    gpio_set_irq_enabled(sensor_center.echoPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    // gpio_set_irq_enabled(LEFT_ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    // gpio_set_irq_enabled(RIGHT_ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

    // Set up the callback function for the echo pin(s)
    gpio_add_raw_irq_handler(CENTER_ECHO_PIN, echoUltraSonic_callback_center);
    // gpio_add_raw_irq_handler(LEFT_ECHO_PIN, echoUltraSonic_callback_left);
    // gpio_add_raw_irq_handler(RIGHT_ECHO_PIN, echoUltraSonic_callback_right);

    //enable interrupt master
    irq_set_enabled(IO_IRQ_BANK0,true);

   
    while (true)
    {
        tight_loop_contents();
        if (sensor_center.distance <= MIN_DISTANCE && sensor_center.distance>0 && sensor_center.timeOut == false)
        {
            gpio_put(LED_PIN, 1);
             
        }
        else
        {
            gpio_put(LED_PIN, 0);
        }
    }
}