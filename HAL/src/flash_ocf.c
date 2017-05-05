#include "system.h"
#if defined(FLASH_WRITER_OCF_ENABLED) && defined(__ALTERA_ONCHIP_FLASH)

#include <errno.h>
#include <string.h>
#include <alloca.h>
#include "sys/alt_flash.h"
#include "flash_file.h"
#include "altera_onchip_flash.h"

/*
References:
https://www.altera.com/en_US/pdfs/literature/hb/max-10/ug_m10_ufm.pdf
https://www.altera.com/en_US/pdfs/literature/hb/max-10/ug_m10_config.pdf
         ______________________________________
         |         Pages per sector           |________________________
_________|     ufm     |   image1    | image0 | Page | Total size[kb] |
| Device | ufm1 | ufm0 | cfm2 | cfm1 |  cfm0  | [kb] |  UFM  |  CFM   |
+--------+------+------+------+------+--------+------+-------+--------+
| 10M02  |    3 |    3 |    0 |    0 |     34 |   16 |    96 |    544 |
| 10M04  |    0 |    8 |   41 |   29 |     70 |   16 | 1,248 |  2,240 |
| 10M08  |    8 |    8 |   41 |   29 |     70 |   16 | 1,376 |  2,240 |
| 10M16  |    4 |    4 |   38 |   28 |     66 |   32 | 2,368 |  4,224 |
| 10M25  |    4 |    4 |   52 |   40 |     92 |   32 | 3,200 |  5,888 |
| 10M40  |    4 |    4 |   48 |   36 |     84 |   64 | 5,888 | 10,752 |
| 10M50  |    4 |    4 |   48 |   36 |     84 |   64 | 5,888 | 10,752 |
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
___________________________________________________________________
|        Configuration         |            UFM space             |
| Images | Compress | Mem Init | ufm1 | ufm0 | cfm2 | cfm1 | cfm0 |
+--------+----------+----------+------+------+------+------+------+
| Dual   |   Yes    |   No     | UFM  | UFM  |      |      |      |
| Single |   No     |   No     | UFM  | UFM  | UFM  |      |      |
| Single |   Yes    |   No     | UFM  | UFM  | UFM  | UFM  |      |
| Single |   No     |   Yes    | UFM  | UFM  |      |      |      |
| Single |   Yes    |   Yes    | UFM  | UFM  |      |      |      |
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

int flash_writer_ocf_init(const char *dirname)
{
	alt_onchip_flash_dev *flash;
	int dir_len;
	char *name;
	int result;

	flash = (alt_onchip_flash_dev *)alt_flash_open_dev(FLASH_WRITER_OCF_NAME);
	if (!flash) {
		return -ENODEV;
	}

	dir_len = strlen(dirname);
	if (dirname[dir_len - 1] == '/') {
		--dir_len;
	}
	name = (char *)alloca(dir_len + 1 + 6 + 1);
	memcpy(name, dirname, dir_len);

	if (flash->sector1_enabled) {
		strcpy(name + dir_len, "/ufm1");
		result = flash_file_reg(
				&flash->dev, name,
				flash->sector1_start_addr,
				flash->sector1_end_addr,
				FLASH_WRITER_OCF_READ_ONLY);
		if (result < 0) {
			return result;
		}
	}

	if (flash->sector2_enabled) {
		strcpy(name + dir_len, "/ufm0");
		result = flash_file_reg(
				&flash->dev, name,
				flash->sector2_start_addr,
				flash->sector2_end_addr,
				FLASH_WRITER_OCF_READ_ONLY);
		if (result < 0) {
			return result;
		}
	}

	if (flash->sector1_enabled || flash->sector2_enabled) {
		strcpy(name + dir_len, "/ufm");
		result = flash_file_reg(
				&flash->dev, name,
				flash->sector1_enabled ? flash->sector1_start_addr : flash->sector2_start_addr,
				flash->sector2_enabled ? flash->sector2_end_addr : flash->sector1_end_addr,
				FLASH_WRITER_OCF_READ_ONLY);
		if (result < 0) {
			return result;
		}
	}

	if (flash->sector3_enabled) {
		strcpy(name + dir_len, "/cfm2");
		result = flash_file_reg(
				&flash->dev, name,
				flash->sector3_start_addr,
				flash->sector3_end_addr,
				FLASH_WRITER_OCF_READ_ONLY);
		if (result < 0) {
			return result;
		}
	}

	if (flash->sector4_enabled) {
		strcpy(name + dir_len, "/cfm1");
		result = flash_file_reg(
				&flash->dev, name,
				flash->sector4_start_addr,
				flash->sector4_end_addr,
				FLASH_WRITER_OCF_READ_ONLY);
		if (result < 0) {
			return result;
		}
	}

	if (flash->sector3_enabled || flash->sector4_enabled) {
		strcpy(name + dir_len, "/image1");
		result = flash_file_reg(
				&flash->dev, name,
				flash->sector3_enabled ? flash->sector3_start_addr : flash->sector4_start_addr,
				flash->sector4_enabled ? flash->sector4_end_addr : flash->sector3_end_addr,
				FLASH_WRITER_OCF_READ_ONLY);
		if (result < 0) {
			return result;
		}
	}

	if (flash->sector5_enabled) {
		strcpy(name + dir_len, "/cfm0");
		result = flash_file_reg(
				&flash->dev, name,
				flash->sector5_start_addr,
				flash->sector5_end_addr,
				FLASH_WRITER_OCF_READ_ONLY);
		if (result < 0) {
			return result;
		}

		strcpy(name + dir_len, "/image0");
		result = flash_file_reg(
				&flash->dev, name,
				flash->sector5_start_addr,
				flash->sector5_end_addr,
				FLASH_WRITER_OCF_READ_ONLY);
		if (result < 0) {
			return result;
		}
	}

	return 0;
}

#endif  /* FLASH_WRITER_OCF_ENABLED && __ALTERA_ONCHIP_FLASH */
