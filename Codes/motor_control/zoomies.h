
#ifndef MOTOR_H
#define MOTOR_H

#include "pico/stdlib.h"
#include <stdio.h>
#include <inttypes.h>
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/timer.h"

extern const uint16_t PWM_COUNTER;
extern const uint16_t SPEED_CALC_FREQ; //In ms
extern const uint16_t PID_FREQ; //In ms
extern const int TARGET_SPEED;
extern const float Kp;   
extern const float Ki;
extern const float Kd;

//MACROS
#define pinENA 8 //PWM_CHAN_A
#define pinENB 9 //PWM_CHAN_B
#define pinIN1 2
#define pinIN2 3
#define pinIN3 4
#define pinIN4 5
#define pinENCA 6
#define pinENCB 7

//Function Prototypes

//Encoder
void encoderAIRQ();
void encoderBIRQ();
bool calcSpeed(repeating_timer_t *rt);
bool initEncoders(repeating_timer_t *rt);

bool computeError(repeating_timer_t *rt);
bool computeErrorA();
bool computeErrorB();
bool initPWM(repeating_timer_t *rt);
void resetVariables();

void initWheels();
int64_t moveForwards(int time);
int64_t moveBackwards(int time);
int64_t moveAntiClockWise(int time);
int64_t moveClockWise(int time);
int64_t stopMovement(alarm_id_t id, void *user_data);

extern uint counterA;
extern uint counterB;
extern uint speedA;
extern uint speedB;
extern uint pwmA;
extern uint pwmB;

extern int integralA;
extern int lastErrorA;
extern absolute_time_t lastUpdateA;

extern int integralB;
extern int lastErrorB;
extern absolute_time_t lastUpdateB;

#endif
