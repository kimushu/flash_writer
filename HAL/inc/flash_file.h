#ifndef __FLASH_FILE_H__
#define __FLASH_FILE_H__

#include "sys/alt_flash_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int flash_file_reg(alt_flash_fd *flash, const char *name, alt_u32 start, alt_u32 end, int read_only);

#ifdef __cplusplus
}	/* extern "C" */
#endif

#endif  /* __FLASH_FILE_H__ */
