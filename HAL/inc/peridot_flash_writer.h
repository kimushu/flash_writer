#ifndef __PERIDOT_FLASH_WRITER_H__
#define __PERIDOT_FLASH_WRITER_H__

#ifdef __cplusplus
extern "C" {
#endif

extern int peridot_flash_writer_init(void);

#define PERIDOT_FLASH_WRITER_INSTANCE(name, state) extern int alt_no_storage

#define PERIDOT_FLASH_WRITER_INIT(name, state) \
	peridot_flash_writer_init()

#ifdef __cplusplus
}	/* extern "C" */
#endif

#endif  /* __PERIDOT_FLASH_WRITER_H__ */
