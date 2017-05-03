#include "flash_writer.h"
#include "system.h"

int flash_writer_init(void)
{
	const char *root = FLASH_WRITER_ROOT_NAME;
	int result;

#ifdef FLASH_WRITER_OCF_ENABLED
	extern int flash_writer_ocf_init(const char *);
	result = flash_writer_ocf_init(root);
	if (result < 0) {
		return result;
	}
#endif

	return 0;
}

