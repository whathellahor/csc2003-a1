/************************************************************************
 * PICO TO HC05
 * PASSWORD: 1234
*************************************************************************/
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "pico/types.h"
#include "hardware/irq.h"

// STATIC CHARACTER RECEIVED
static int chars_rxed = 0;

// RX interrupt handler
void on_uart_rx() {

    while (uart_is_readable(uart0)) {
        uart_getc(uart0);
        // Can we send it back?
        if (uart_is_writable(uart0)) {
            // Change it slightly first!
            uart_putc(uart0, 'R');
        }
        chars_rxed++;
    }
}

// MAIN PROGRAM ENTRY
int main() {

    // INIT UART | UART 0 | TX_PIN 0 | RX_PIN 1
    uart_init(uart0, 9600);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);

    // RESET HC05, HOLD BUTTON AND CONNECT VSYS AND ENABLE BELOW CODE
    // sleep_ms(10000);
    // uart_puts(uart0, "AT\n\r");
    // sleep_ms(2000);
    // uart_puts(uart0, "AT+ORGL\n\r");
    // sleep_ms(2000);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(uart0, false, false);

    // Set our data format
    uart_set_format(uart0, 8, 1, UART_PARITY_NONE);

    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(uart0, true);

    // Set up a RX interrupt
    // We need to set up the handler first
    // Select correct interrupt for the UART we are using
    int UART_IRQ = UART0_IRQ;

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(uart0, true, false);

    // FOREVER LOOP
    while (true) {
    }

}
