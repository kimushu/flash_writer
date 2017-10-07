#include "flash_file.h"
#include "sys/alt_flash_dev.h"
#include "sys/alt_dev.h"
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum {
	FLASH_FILE_WRONLY = (1<<0),
	FLASH_FILE_DIRTY  = (1<<1),
};

typedef struct {
	alt_dev dev;
	alt_flash_fd *flash;
	alt_u32 start;
	alt_u32 end;
	alt_u8 start_region;
	alt_u8 end_region;
	alt_u16 readonly;
} flash_file_dev;

typedef struct {
	int offset;
	int flags;
	int block_size;
	alt_u8 data[0];
} flash_file_buf;

/**
 * drain buffered data to flash device
 */
static int flash_file_drain(alt_fd *fd)
{
	flash_file_dev *dev = (flash_file_dev *)fd->dev;
	flash_file_buf *buf = (flash_file_buf *)fd->priv;
	int result;

	if ((buf->flags & FLASH_FILE_DIRTY) == 0) {
		return 0;
	}

	result = dev->flash->write(
			dev->flash,
			dev->start + (buf->offset & ~(buf->block_size - 1)),
			buf->data,
			buf->block_size
	);
	if (result < 0) {
		return result;
	}
	buf->flags &= ~FLASH_FILE_DIRTY;
	return 0;
}

/**
 * open() for flash special file
 */
static int flash_file_open(alt_fd *fd, const char *name, int flags, int mode)
{
	flash_file_buf *buf;
	flash_file_dev *dev = (flash_file_dev *)fd->dev;
	int block_size;
	int write_only = 0;

	if (((flags & O_ACCMODE) + 1) & _FWRITE) {
		// Write mode
		if (dev->readonly) {
			return -EACCES;
		}
		if (!(((flags & O_ACCMODE) + 1) & _FREAD)) {
			// Write only
			write_only = 1;
		}
	}
	if (flags & O_EXCL) {
		// Cannot opened for exclusive creation
		return -EACCES;
	}

	if (dev->start_region >= 0) {
		block_size = dev->flash->region_info[dev->start_region].block_size;
	} else {
		block_size = 1;
	}
	buf = (flash_file_buf *)malloc(sizeof(*buf) + ((((flags & O_ACCMODE) + 1) & _FWRITE) ? block_size : 0));
	if (!buf) {
		return -ENOMEM;
	}
	buf->offset = 0;
	buf->flags = (write_only ? FLASH_FILE_WRONLY : 0);
	buf->block_size = block_size;
	if (((flags & O_ACCMODE) + 1) & _FWRITE) {
		memset(buf->data, 0xff, block_size);
	}
	fd->priv = (alt_u8 *)buf;
	return 0;
}

/**
 * close() for flash special file
 */
static int flash_file_close(alt_fd *fd)
{
	int result = flash_file_drain(fd);
	if (result < 0) {
		return result;
	}
	free(fd->priv);
	return 0;
}

/**
 * lseek() for flash special file
 */
static int flash_file_lseek(alt_fd *fd, int ptr, int dir)
{
	flash_file_dev *dev = (flash_file_dev *)fd->dev;
	flash_file_buf *buf = (flash_file_buf *)fd->priv;
	int max_offset = (dev->end + 1 - dev->start);
	int new_offset;

	switch (dir) {
		case SEEK_SET:
			new_offset = ptr;
			break;
		case SEEK_CUR:
			new_offset = buf->offset + ptr;
			break;
		case SEEK_END:
			new_offset = max_offset + ptr;
			break;
		default:
			return -EINVAL;
	}

	if (new_offset < 0) {
		new_offset = 0;
	} else if (new_offset > max_offset) {
		new_offset = max_offset;
	}

	if (((new_offset ^ buf->offset) & ~(buf->block_size - 1)) != 0) {
		int result;
		result = flash_file_drain(fd);
		if (result < 0) {
			return result;
		}
	}
	buf->offset = new_offset;
	return new_offset;
}

/**
 * read() for flash special file
 */
static int flash_file_read(alt_fd *fd, char *ptr, int len)
{
	flash_file_dev *dev = (flash_file_dev *)fd->dev;
	flash_file_buf *buf = (flash_file_buf *)fd->priv;
	alt_u32 dev_offset = dev->start + buf->offset;
	int max_len = (dev->end + 1) - dev_offset;
	int result;

	if (len > max_len) {
		len = max_len;
	}

	if (buf->flags & FLASH_FILE_WRONLY) {
		return -EACCES;
	}

	if (len <= 0) {
		return 0;
	}

	result = dev->flash->read(dev->flash, dev_offset, ptr, len);
	if (result < 0) {
		return result;
	}

	result = flash_file_lseek(fd, len, SEEK_CUR);
	if (result < 0) {
		return result;
	}

	return len;
}

/**
 * write() for flash special file
 */
static int flash_file_write(alt_fd *fd, const char *ptr, int len)
{
	flash_file_dev *dev = (flash_file_dev *)fd->dev;
	flash_file_buf *buf = (flash_file_buf *)fd->priv;
	int max_offset = (dev->end + 1) - dev->start;
	int written = 0;

	while (len > 0) {
		int page_offset = buf->offset & (buf->block_size - 1);
		int page_len = buf->block_size - page_offset;
		int result;

		if (buf->offset >= max_offset) {
			break;
		}
		if (len < page_len) {
			page_len = len;
		}

		memcpy(buf->data + page_offset, ptr, page_len);
		ptr += page_len;
		len -= page_len;
		buf->flags |= FLASH_FILE_DIRTY;
		written += page_len;

		result = flash_file_lseek(fd, page_len, SEEK_CUR);
		if (result < 0) {
			return result;
		}
	}

	return written;
}

/**
 * fstat() for flash special file
 */
static int flash_file_fstat(alt_fd *fd, struct stat *stbuf)
{
	flash_file_dev *dev = (flash_file_dev *)fd->dev;
	flash_file_buf *buf = (flash_file_buf *)fd->priv;

	memset(stbuf, 0, sizeof(*stbuf));
	stbuf->st_size = dev->end + 1 - dev->start;
	stbuf->st_blksize = buf->block_size;
	stbuf->st_blocks = stbuf->st_size / buf->block_size;
	return 0;
}

/**
 * Register special file (block device) for flash device
 */
int flash_file_reg(alt_flash_fd *flash, const char *name, alt_u32 start, alt_u32 end, int read_only)
{
	flash_file_dev *dev;
	int region, start_region;
	int block_size;

	if (read_only) {
		block_size = 0;
		goto create_device;
	}

	for (region = 0; region < flash->number_of_regions; ++region) {
		flash_region *region_info = &flash->region_info[region];
		if ((region_info->offset <= start) &&
			(start < (region_info->offset + region_info->region_size))) {
			block_size = region_info->block_size;
			start_region = region;
			goto found_start_region;
		}
	}
	// Start address exceeds flash device range
	// => Create device as read-only
	block_size = 0;
	goto create_device;

found_start_region:
	for (; region < flash->number_of_regions; ++region) {
		flash_region *region_info = &flash->region_info[region];
		if (end < (region_info->offset + region_info->region_size)) {
			goto found_end_region;
		}
		if (region_info->block_size != block_size) {
			break;
		}
	}
	// End address exceeds flash device range
	// => Create device as read-only
	block_size = 0;
	goto create_device;

found_end_region:
	if ((start & (block_size - 1)) != 0 ||
			((end + 1) & (block_size - 1)) != 0) {
		// Bad alignment
		return -EINVAL;
	}

create_device:
	dev = (flash_file_dev *)malloc(sizeof(*dev) + strlen(name) + 1);
	if (!dev) {
		return -ENOMEM;
	}

	dev->dev.name = (const char *)(dev + 1);
	strcpy((char *)dev->dev.name, name);
	dev->dev.open  = flash_file_open;
	dev->dev.close = flash_file_close;
	dev->dev.read  = flash_file_read;
	dev->dev.write = flash_file_write;
	dev->dev.lseek = flash_file_lseek;
	dev->dev.fstat = flash_file_fstat;
	dev->dev.ioctl = NULL;
	dev->flash = flash;
	dev->start = start;
	dev->end = end;
	dev->start_region = (block_size > 0 ? start_region : -1);
	dev->end_region = (block_size > 0 ? region : -1);
	dev->readonly = (block_size == 0);
	return alt_dev_reg(&dev->dev);
}

