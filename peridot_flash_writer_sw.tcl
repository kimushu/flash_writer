#
# peridot_flash_writer_sw.tcl
#

create_sw_package peridot_flash_writer

set_sw_property version 16.1
set_sw_property auto_initialize true
set_sw_property bsp_subdirectory services
set_sw_property alt_sys_init_priority 1100

#
# Source file listings...
#

add_sw_property c_source HAL/src/flash_file.c
add_sw_property c_source HAL/src/flash_max10_ocf.c
add_sw_property c_source HAL/src/peridot_flash_writer.c

add_sw_property include_source HAL/inc/peridot_flash_writer.h
add_sw_property include_source HAL/inc/flash_file.h
add_sw_property include_directory inc

add_sw_property supported_bsp_type HAL

#
# BSP settings...
#
add_sw_setting quoted_string system_h_define root.name PERIDOT_FLASH_WRITER_ROOT_NAME "/sys/flash" "Root name of PERIDOT flash writer"
add_sw_setting boolean_define_only system_h_define on_chip_flash.enabled PERIDOT_FLASH_WRITER_OCF_ENABLED 0 "Enable altera_onchip_flash for MAX 10 devices"
add_sw_setting quoted_string system_h_define on_chip_flash.name PERIDOT_FLASH_WRITER_OCF_NAME "/dev/altera_onchip_flash_csr" "Device name of altera_onchip_flash CSR"

# End of file
