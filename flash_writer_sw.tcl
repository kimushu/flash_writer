#
# flash_writer_sw.tcl
#

create_sw_package flash_writer

set_sw_property version 16.1
set_sw_property auto_initialize true
set_sw_property bsp_subdirectory services
set_sw_property alt_sys_init_priority 1100

#
# Source file listings...
#

add_sw_property c_source HAL/src/flash_writer.c
add_sw_property c_source HAL/src/flash_file.c
add_sw_property c_source HAL/src/flash_ocf.c
add_sw_property c_source HAL/src/flash_spi.c

add_sw_property include_source HAL/inc/flash_writer.h
add_sw_property include_source HAL/inc/flash_file.h
add_sw_property include_directory inc

add_sw_property supported_bsp_type HAL

#
# BSP settings...
#
add_sw_setting quoted_string system_h_define root.name FLASH_WRITER_ROOT_NAME "/sys/flash" "Root name of flash writer"
add_sw_setting boolean_define_only system_h_define on_chip_flash.enabled FLASH_WRITER_OCF_ENABLED 0 "Enable altera_onchip_flash for MAX 10 devices"
add_sw_setting quoted_string system_h_define on_chip_flash.name FLASH_WRITER_OCF_NAME "/dev/altera_onchip_flash_data" "Device name of altera_onchip_flash CSR"
add_sw_setting boolean system_h_define on_chip_flash.read_only FLASH_WRITER_OCF_READ_ONLY 0 "Set read-only mode for altera_onchip_flash"
add_sw_setting boolean_define_only system_h_define spi.enabled FLASH_WRITER_SPI_ENABLED 0 "Enable SPI flash device"
add_sw_setting quoted_string system_h_define spi.name FLASH_WRITER_SPI_NAME "/dev/xxx" "Name of flash device"
add_sw_setting boolean system_h_define spi.read_only FLASH_WRITER_SPI_READ_ONLY 0 "Set read-only mode for SPI flash access"

# End of file
