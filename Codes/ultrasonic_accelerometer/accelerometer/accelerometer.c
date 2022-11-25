/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include <math.h>
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// By default these devices are on bus address 0x68
static int addr = 0x68;

// absolute_time_t startTime;
// uint startTime;

// not using the default i2c pins. defined values are the selected pins for this project
#define SDA_PIN 16
#define SCL_PIN 17

static absolute_time_t startTime;
float accUp;

// set flag and stable values for when car first power up to store initialized values
bool startFlag = 0;        // car not start yet.
float stableAcc, min, max; // store value for when car first power up
// boolean to check for presence of hump. default value is 0 for no hump detected
bool hump = 0;
// check if car is at the peak of the hump
bool peak = 0;

// functions
void hump_calculation(float Ay, uint time);
bool triggerAcc_callback(struct repeating_timer *t);

#ifdef i2c_default
static void mpu6050_reset()
{
    // Two byte reset. First byte register, second byte data
    // 0x6B is power management 1
    // Reset value is 0x00 for most registers (apart from 0x40, 0x68)
    uint8_t buf[] = {0x6B, 0x00};
    i2c_write_blocking(i2c_default, addr, buf, 2, false);
}

static void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp)
{
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.

    uint8_t buffer[6];

    // 3B is the register for accelerometer(High) output
    //  Start reading acceleration registers from register 0x3B for 6 bytes
    //(Registers  59 to 64)
    uint8_t val = 0x3B;
    // i2c_write blocking: attempt to write specified number of bytes to address.
    i2c_write_blocking(i2c_default, addr, &val, 1, true); // true to keep master control of bus
    // i2c_read blocking: Attempt to read specified number of bytes from address,
    i2c_read_blocking(i2c_default, addr, buffer, 6, false);

    for (int i = 0; i < 3; i++)
    {
        accel[i] = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
    }
}
#endif

int main()
{
    stdio_init_all();
#if !defined(i2c_default) || !defined(SDA_PIN) || !defined(SCL_PIN)
#warning i2c/mpu6050_i2c example requires a board with I2C pins
    puts("Default I2C pins were not defined");
#else

    // Set pin that I2C0 will use. SDA and SCL pins 16 and 17 respectively
    i2c_init(i2c_default, 400 * 1000);
    // Set selected SDA and SCL pins to use the I2C function on the pico
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    // set SDA SCL pins as pull up resistor
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(SDA_PIN, SCL_PIN, GPIO_FUNC_I2C));

    mpu6050_reset();

    // call timer to iterate the function to check for the hump
    repeating_timer_t triggerTimer;
    add_repeating_timer_ms(500, triggerAcc_callback, NULL, &triggerTimer);

    while (1)
    {
        tight_loop_contents();
    }

#endif
    return 0;
}

void hump_calculation(float Acc, uint time)
{
    float height = 0;
    height = Acc / 0.14; //0.14 is approximate ratio of change in Y-axis to 1cm
    //height = Acc * pow(time, 2); // formula to calculate height of the hump
    // printf("time going up the hump: %lld\n", time);
    printf("Height is: %.2f cm\n\n", height);
    // return height;
}

bool triggerAcc_callback(repeating_timer_t *t)
{
    // array variables to store accelerometer values
    int16_t acceleration[3], gyro[3], temp;
    // calling the method to obtain the raw values that the acclerometer read
    mpu6050_read_raw(acceleration, gyro, &temp);

    float Ax, Ay, Az;
    float Gx, Gy, Gz;

    // uint time;
    // 16384
    //+- 4g Full Scale Range. 8192 LSB Sensitivity
    Ax = acceleration[0] / 8192.0;
    Ay = acceleration[1] / 8192.0;
    Az = acceleration[2] / 8192.0;

    if (startFlag == 0)
    {

        stableAcc = Ay;
        // set the range of values. if Ay is within this range, it's on flat ground
        min = stableAcc - 0.1;
        max = stableAcc + 0.1;

        startFlag = 1; // car is moving. dont come in here again
    }

    if (hump == 0)
    {
        if (((Ay - min) * (Ay - max)) <= 0)
        {
            printf("moving on flat ground\n");
        }
        else if (Ay > 0)
        {
            // to get the snapshot of the time the moment the car goes up the hump
            startTime = get_absolute_time();
            printf("Hump detected. Going up\n");
            accUp = Ay; // assign the value of the acceleration at the X axis to the global value the moment the car goes up the hump
            hump = 1;   // hump detected, set boolean to true
            printf("hump value: %d\n", hump);
        }
    }

    if (hump == 1 && Ay < stableAcc)
    {
        absolute_time_t endTime = get_absolute_time();
        uint time = absolute_time_diff_us(startTime, endTime);
        time = time / 1000000; // convert time captured from micro-seconds to seconds
        hump_calculation(accUp, time);
        printf("Check time in secs: %d\n", time); // call the function to calculate the height of the hump
        hump = 0;                                 // set boolean value back to 0;
        printf("down slope: %d\n", hump);
    }

    printf("Raw Acceleration is. X = %.2f, Y = %.2f, Z = %.2f\n", Ax, Ay, Az);

    return true;
}
