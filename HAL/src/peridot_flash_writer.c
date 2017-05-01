#include "peridot_flash_writer.h"
#include "system.h"

int peridot_flash_writer_init(void)
{
	const char *root = PERIDOT_FLASH_WRITER_ROOT_NAME;
	int result;

#ifdef PERIDOT_FLASH_WRITER_OCF_ENABLED
	extern int peridot_flash_writer_ocf_init(const char *);
	result = peridot_flash_writer_ocf_init(root);
	if (result < 0) {
		return result;
	}
#endif

	return 0;
}

