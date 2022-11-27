// INCLUDE HEADER FILE
#include "comm_mod.h"

/************************************************************************************
 * INSTRUCTIONS:
 * SET DEBUG IN HEADER FILE TO 0 FOR PRODUCTION
 * SET AND GET VALUES FROM car_data GLOBAL STRUCT
*************************************************************************************/



// UART CONFIG
void config_uart() {

    // SET BAUD RATE: 115200, DATABITS: 8, STOPBITS: 1, PARITY: NONE
    // USING PICO UART0
    // TX PIN 0, RX PIN 1
    uart_init(UART_ID, 115200);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);

    // FOR DEBUG PURPOSES, DEBUGGING THROUGH UART1
    // SET BAUD RATE: 115200, DATABITS: 8, STOPBITS: 1, PARITY: NONE
    // USING PICO UART1
    // TX PIN 4, RX PIN 5
    if (DEBUG == 1) {
        uart_init(uart1, 115200);
        gpio_set_function(4, GPIO_FUNC_UART);
        gpio_set_function(5, GPIO_FUNC_UART);
        uart_set_translate_crlf(uart1, true);
        uart_set_format(uart1, 8, 1, UART_PARITY_NONE);
    }
}

// GET RESPONSE BACK FROM AT COMMANDS
int get_at_response() {
    // INIT LOOP
    int data_count = 0;
    while (data_count < BUFFER_LEN - 1) {
        // ARBITRARY NUMBER FOR UART READABLE TIMEOUT
        if (uart_is_readable_within_us(UART_ID, 10000)) {
            // INCREMENT DATA COUNT AND GET UART DATA BACK FROM AT
            data[data_count++] = uart_getc(UART_ID);
            // FOR DEBUG
            uart_putc(uart1, data[data_count - 1]);
        } else {
            // BREAK IF NO MORE DATA
            break;
        }
    }
    // TERMINATING
    data[data_count] = 0;
    return data_count;
}

// CHECK AT RESPONSE MATCH
int check_at_response(int len_to_check, char str_to_check[])
{
    // INITIATE LOOP TO CHECK TILL LEN_TO_CHECK
    for (int i = 0; i < len_to_check; i++)
    {
        // ARBITRARY NUMBER FOR UART READABLE TIMEOUT
        if (uart_is_readable_within_us(UART_ID, 1000 * 1000))
        {
            // GET RESPONSE
            get_at_response();
            // BREAK IF AT RESPONSE MATCH STR_TO_CHECK
            if (strstr(data, str_to_check))
                // RETURN INDEX
                return i;
        }
    }
    return -1;
}

// GET AT STATUS
void get_at_status() {
    uint8_t command[] = "AT\r\n";
    uart_write_blocking(UART_ID, command, strlen(command));
    get_at_response();
}

// SET ESP01 MODE, 1: STATION, 2: HOST, 3: BOTH
void set_esp_mode(int mode) {
    uint8_t command[128]; 
    sprintf(command, "AT+CWMODE=%d\r\n", mode);
    uart_write_blocking(UART_ID, command, strlen(command));
    get_at_response();
}

// SET CONNECTION OF ESP01 TO ROUTER
void set_connection(char ssid[], char password[]) {
    uint8_t command[256];
    sprintf(command, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
    uart_write_blocking(UART_ID, command, strlen(command));
    check_at_response(20, "OK");
}

// GET IP OF ESP01
void get_ip() {
    uint8_t command[] = "AT+CIFSR\r\n";
    uart_write_blocking(UART_ID, command, strlen(command));
    check_at_response(20, "OK");
}

// START ESP01 SERVER, 1 FOR DEBUG, 0 FOR NON-DEBUG
int start_server() {
    
    // TEMP BUFFER FOR ID
    char temp[256];
    char id[10];

    // ENABLE MULTIPLE CONNECTIONS
    uart_write_blocking(UART_ID, "AT+CIPMUX=1\r\n", 13);
    if (check_at_response(10, "OK") < 0) {
        // SET MODE TO MULTIPLE CONNECTIONS FAILED
        return -1;
    }

    // START ESP01 SERVER
    uart_write_blocking(UART_ID, "AT+CIPSERVER=1,80\r\n", 19);
    if (check_at_response(10, "OK") < 0) {
        // START SERVER FAILED
        return -1;
    }

    // IPD RESPONSE
    // char ipd[5] = "+IPD";

    // INIT SERVER LOOP
    while (1) {
        // GET CONNECTION IPD
        if (check_at_response(1, "+IPD") < 0) {
            continue;
        }

        // SET ID
        // SET POINTER TO START OF FIRST OCCURENCE OF "+IPD"
        char *b = strstr(data, "+IPD");
        b += 5;
        strncpy(temp, b, sizeof(temp));
        char *e = strstr(temp, ",");
        int d = e - temp;
        memset(id, '\0', sizeof(id));
        strncpy(id, temp, d);

        // DUMMY DATA - FOR DEBUG
        if (DEBUG == 1) {
            static int count;
            if (count % 2 == 0) {
                sprintf(car_data.barcode, "%s", "DATASET 1");
                sprintf(car_data.map_data, "%s", "111100001111000");
                car_data.hump_detected = 1;
                car_data.turn_detected = 1;
                car_data.hump ++;
                car_data.turn ++;
                car_data.speed = 0;
            } else {
                sprintf(car_data.barcode, "%s", "DATASET 2");   
                sprintf(car_data.map_data, "%s", "111100001111111");     
                car_data.hump_detected = 0;
                car_data.turn_detected = 0;
                car_data.distance += 10;
                car_data.speed = 10;
            }
            count ++;
        }

        // DATA OUT TO CLIENTS
        char data_out[BUFFER_LEN];
        sprintf(data_out, "HTTP/1.0 200 OK\r\nServer: Pico\r\nAccess-Control-Allow-Origin: *\r\nContent-type: application/json\r\n\r\n{\"car_hump\":\"%d\", \"car_turn\":\"%d\", \"hump_number\":\"%d\", \"turn_number\": \"%d\", \"car_barcode\":\"%s\", \"car_distance\":\"%d\", \"car_speed\":\"%d\", \"map_data\":\"%s\"}\r\n", car_data.hump_detected, car_data.turn_detected, car_data.hump, car_data.turn, car_data.barcode, car_data.distance, car_data.speed, car_data.map_data);

        // GETTING CORRECT ID TO SEND RESPONSE
        uint8_t command[128];
        sprintf(command, "AT+CIPSEND=%s,%d\r\n", id, strlen(data_out));
        uart_write_blocking(UART_ID, command, strlen(command));
        // IF SUCCESS, ESP01 WILL INDICATE ">" TO SEND DATA
        if (check_at_response(10, ">") < 0) {
            return -1;
        }

        // SENDING DATA
        uart_write_blocking(UART_ID, data_out, strlen(data_out));
        // CHECK IF SENDING SUCCEEDED
        if (check_at_response(10, "OK") < 0) {
            return -1;
        }

        // CLOSE SENDING CONNECTION
        sprintf(command, "AT+CIPCLOSE=%s\r\n", id);
        uart_write_blocking(UART_ID, command, strlen(command));
        // CHECK IF SUCCESS
        if (check_at_response(10, "OK") < 0) {
            return -1;
        }
    }

    return 0;
}

// INIT COMMS BUNDLE | ESP MODE: 1-3 | DEBUG MODE: 1 FOR DEBUG
void init_comms(int esp_mode, char ssid[], char password[]) {
    // INIT STDIO
    stdio_init_all();
    sleep_ms(1000);
    
    // CONFIG UART
    config_uart();
    sleep_ms(1000);

    // CONFIG WIFI SETTINGS
    set_esp_mode(esp_mode);
    set_connection(ssid, password);

    // GET IP
    get_ip();

    // START SERVER
    start_server();
    sleep_ms(1000);
}

// // MAIN PROGRAM ENTRY
// int main() {
//     init_comms(1, "pico_test1", "testtest"); // ESP MODE: USE 1, SSID, PASSWORD
// }
