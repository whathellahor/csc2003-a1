add_executable(
        barcode barcode.c
        )

pico_add_extra_outputs(barcode)

#Edit below for additional libraries to link
target_link_libraries(barcode pico_stdlib pico_multicore hardware_adc hardware_i2c hardware_pwm)

pico_enable_stdio_usb (barcode 1)
pico_enable_stdio_uart(barcode 0)