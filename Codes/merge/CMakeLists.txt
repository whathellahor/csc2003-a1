add_executable(
        car car.c
        ../motor_control/zoomies.h ../motor_control/zoomies.c
        ../ultrasonic_accelerometer/ultrasonic/ultrasonic.h ../ultrasonic_accelerometer/ultrasonic/ultrasonic.c
        ../ultrasonic_accelerometer/accelerometer/accelerometer.h ../ultrasonic_accelerometer/accelerometer/accelerometer.c
        ../infrared/barcode/barcode.h ../infrared/barcode/barcode.c
        ../mapping_navigation/mapandnav ../mapping_navigation/mapandnav.c
        ../communications/esp-uart/comm_mod.h ../communications/esp-uart/comm_mod.c
)

pico_add_extra_outputs(car)

#Edit below for additional libraries to link
target_link_libraries(car pico_stdlib hardware_pwm hardware_gpio hardware_i2c hardware_timer hardware_adc pico_multicore)

pico_enable_stdio_usb(car 1)
pico_enable_stdio_uart(car 1)