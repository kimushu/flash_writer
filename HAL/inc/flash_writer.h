#ifndef __FLASH_WRITER_H__
#define __FLASH_WRITER_H__

#ifdef __cplusplus
extern "C" {
#endif

extern int flash_writer_init(void);

#define FLASH_WRITER_INSTANCE(name, state) extern int alt_no_storage

#define FLASH_WRITER_INIT(name, state) \
	flash_writer_init()

#ifdef __cplusplus
}	/* extern "C" */
#endif

#endif  /* __FLASH_WRITER_H__ */
