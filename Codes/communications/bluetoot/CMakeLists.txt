add_executable(bluetoot
        bluetoot.c
        )

# pull in common dependencies
target_link_libraries(bluetoot pico_stdlib hardware_uart)

# # enable usb output, disable uart output
pico_enable_stdio_usb(bluetoot 1)
pico_enable_stdio_uart(bluetoot 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(bluetoot)

# add url via pico_set_program_url
example_auto_set_url(bluetoot)
