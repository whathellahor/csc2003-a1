add_executable(
        accelerometer accelerometer.c
        )



#Edit below for additional libraries to link
target_link_libraries(accelerometer pico_stdlib hardware_i2c)

pico_add_extra_outputs(accelerometer)

pico_enable_stdio_usb(accelerometer 1)
pico_enable_stdio_uart(accelerometer 0)