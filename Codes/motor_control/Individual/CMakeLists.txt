add_executable(
        motorModule motorModule.c
        )

pico_add_extra_outputs(motorModule)

#Edit below for additional libraries to link
target_link_libraries(motorModule pico_stdlib hardware_pwm)

pico_enable_stdio_usb(motorModule 1)
pico_enable_stdio_uart(motorModule 0)
