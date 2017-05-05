#include "system.h"
#ifdef FLASH_WRITER_SPI_ENABLED

#include <errno.h>
#include <string.h>
#include <alloca.h>
#include "sys/alt_flash.h"
#include "flash_file.h"

/**
 * Initialize SPI flash special file
 */
int flash_writer_spi_init(const char *dirname)
{
	alt_flash_fd *flash;
	int dir_len;
	char *name;

	flash = alt_flash_open_dev(FLASH_WRITER_SPI_NAME);
	if (!flash) {
		return -ENODEV;
	}

	dir_len = strlen(dirname);
	if (dirname[dir_len - 1] == '/') {
		--dir_len;
	}
	name = (char *)alloca(dir_len + 1 + 3 + 1);
	memcpy(name, dirname, dir_len);
	strcpy(name + dir_len, "/spi");

	return flash_file_reg(
			flash, name, 0, flash->length - 1,
			FLASH_WRITER_SPI_READ_ONLY);
}

#endif  /* FLASH_WRITER_SPI_ENABLED */
