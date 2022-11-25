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

// struct function for sensors
extern struct Sensors{
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

//Prototypes
void initializeSensorValues();
void setupUltrasonicPins();
void Smoothing(struct Sensors *sensor);
void setTriggerPinHigh(uint trigPin);
void setTriggerPinLow(uint trigPin);
void setEchoPinHigh(uint echoPin);
void setEchoPinLow(uint echoPin);
void getDistance(uint timeDifference, struct Sensors *sensor);
void echoUltraSonic_callback_center();
void echoUltraSonic_callback_left();
void echoUltraSonic_callback_right();
bool triggerUltraSonic_callback(struct repeating_timer *t);


// IRQ handlers
int64_t triggerAlarm_callback(alarm_id_t id, void *user_data); // shooting pulse IRQ
int64_t EchoMaxAlarm_callback_center(alarm_id_t id, void *user_data); // center car IRQ
int64_t EchoMaxAlarm_callback_left(alarm_id_t id, void *user_data); // left side of car IRQ
int64_t EchoMaxAlarm_callback_right(alarm_id_t id, void *user_data); // right side of car IRQ


