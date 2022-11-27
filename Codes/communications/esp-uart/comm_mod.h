// IMPORT GUARD START
#ifndef COMM_H
#define COMM_H

// DEFINE IMPORTS
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <string.h>

// DEFINE GLOBAL CONSTANTS
#define UART_ID uart0
#define BUFFER_LEN 512
#define DEBUG 0 // CHANGE TO 0 TO OFF DEBUG, 1 TO ON DEBUG

// GLOBAL STRUCT
typedef struct {
    uint8_t hump; // TOTAL NUMBER OF HUMPS
    uint8_t turn; // TOTAL NUMBER OF TURNS 
    uint8_t hump_detected; // 1 FOR DETECTED, 0 FOR NOT DETECTED
    uint8_t turn_detected; // 1 FOR DETECTED, 0 FOR NOT DETECTED
    uint8_t barcode[100]; // BARCODE, ONCE BARCODE DETECTED, FIX THE VALUE UNTIL NEXT BARCODE
    uint8_t distance; // TOTAL DISTANCE TRAVELLED
    uint8_t speed; // CURRENT SPEED
    char map_data[100]; // ARBITARY NUMBER, SET ACCORDING TO ACTUAL TILES
} Car;

// FUNCTION PROTOTYPES
void config_uart();
int get_at_response();
int check_at_response(int len_to_check, char str_to_check[]);
void get_at_status();
void set_esp_mode(int mode);
void set_connection(char ssid[], char password[]);
void get_ip();
int start_server();
void init_comms(int esp_mode, char ssid[], char password[]);

// GLOBAL VARIABLES
// CAR DATA
Car car_data = {
    0, // HUMP NUMBER
    0, // TURN NUMBER
    0, // HUMP DETECTED
    0, // TURN DETECTED
    "", // BARCODE
    0, // DISTANCE
    0, // SPEED
    "", // MAP DATA
};
// DATA BUFFER
uint8_t data[BUFFER_LEN];

// IMPORT GUARD END
#endif