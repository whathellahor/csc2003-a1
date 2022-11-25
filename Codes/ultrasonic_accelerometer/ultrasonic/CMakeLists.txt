add_executable(
        ultrasonic ultrasonic.c
        )

pico_add_extra_outputs(ultrasonic)

#Edit below for additional libraries to link
target_link_libraries(ultrasonic pico_stdlib hardware_pwm)

pico_enable_stdio_usb (ultrasonic 1)
pico_enable_stdio_uart(ultrasonic 0)