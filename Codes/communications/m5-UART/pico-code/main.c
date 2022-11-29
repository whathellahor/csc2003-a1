#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include <stdio.h>

#define UART_ID uart0               // Configuring uart0 for pico
#define BAUD_RATE 115200            // Setting BAUD_RATE
#define DATA_BITS 8                 // Setting DATA_BITS
#define STOP_BITS 1                 // Setting STOP_BITS
#define PARITY    UART_PARITY_NONE  // Setting PARTIY
#define UART_TX_PIN 0               // Configuring GP0 as TX
#define UART_RX_PIN 1               // Configuring GP1 as RX

int8_t x = 0;   // Variable for data that will be sent.

// RX interrupt handler
void on_uart_rx() {
    // Infinite loop to check for data in RX
    while (uart_is_readable(UART_ID)) {
        char text[6];               // Create a char variable to store value
        sprintf(text, "%d\n", x);   // Using sprintf to convert int8_t to char
        uart_puts(UART_ID, text);   // Send data via TX
        x += 1;                     // Increment value
        if (x == 20){               // Reset value
            x = 0;
        }
    }
}

int main() {
    stdio_init_all();               // To allow data to print in PUTTY
    uart_init(UART_ID, BAUD_RATE);  // Confiuring basic baud rate

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART); // Setting GP0 as TX
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART); // Setting GP1 as RX

    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY); // Formatting data

    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;    // Setting up the handler

    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);    // Setting up handler
    irq_set_enabled(UART_IRQ, true);                    // Enabling interrupt handlers

    uart_set_irq_enables(UART_ID, true, false);         // Allow UART to send interrupts

    while (1){
        tight_loop_contents();
    }
}