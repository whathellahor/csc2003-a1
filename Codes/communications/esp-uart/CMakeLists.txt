add_executable(comm_mod
        comm_mod.c
        )

# pull in common dependencies
target_link_libraries(comm_mod pico_stdlib hardware_uart)

# # enable usb output, disable uart output
# pico_enable_stdio_usb(comm_mod 0)
# pico_enable_stdio_uart(comm_mod 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(comm_mod)

