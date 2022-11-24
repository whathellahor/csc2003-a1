add_executable(
        zoomies zoomies.c
        )

pico_add_extra_outputs(zoomies)

#Edit below for additional libraries to link
target_link_libraries(zoomies pico_stdlib hardware_pwm)

pico_enable_stdio_usb(zoomies 1)
pico_enable_stdio_uart(zoomies 0)
