/*
 * operations on IDE disk.
 */

#include "serv.h"
#include <drivers/dev_disk.h>
#include <lib.h>
#include <mmu.h>

// Overview:
//  read data from IDE disk. First issue a read request through
//  disk register and then copy data from disk buffer
//  (512 bytes, a sector) to destination array.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  dst: destination for data read from IDE disk.
//  nsecs: the number of sectors to read.
//
// Post-Condition:
//  Panic if any error occurs. (you may want to use 'panic_on')
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_OPERATION_READ',
//  'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS', 'DEV_DISK_BUFFER'
void ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		/* Exercise 5.3: Your code here. (1/2) */
    	uint32_t status = 0;
		uint32_t cur_offset = begin + off;
		uint32_t read_flag = DEV_DISK_OPERATION_READ;
		//diskno
      	panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_ID, sizeof(temp)));
		//disk offset
      	panic_on(syscall_write_dev(&cur_offset, DEV_DISK_ADDRESS + DEV_DISK_OFFSET, sizeof(cur_offset)));
		//0 stands for read
      	panic_on(syscall_write_dev(&read_flag, DEV_DISK_ADDRESS + DEV_DISK_START_OPERATION, sizeof(read_flag)));
		//get return value
      	panic_on(syscall_read_dev(&status, DEV_DISK_ADDRESS + DEV_DISK_STATUS, sizeof(status)));
    	if (status == 0) {
      		user_panic("disk read error 5\n");
    	}
		//从缓冲区读出要读的数据
      	panic_on(syscall_read_dev(dst + off, DEV_DISK_ADDRESS + DEV_DISK_BUFFER, DEV_DISK_BUFFER_LEN));
	}
}

// Overview:
//  write data to IDE disk.
//
// Parameters:
//  diskno: disk number.
//  secno: start sector number.
//  src: the source data to write into IDE disk.
//  nsecs: the number of sectors to write.
//
// Post-Condition:
//  Panic if any error occurs.
//
// Hint: Use syscalls to access device registers and buffers.
// Hint: Use the physical address and offsets defined in 'include/drivers/dev_disk.h':
//  'DEV_DISK_ADDRESS', 'DEV_DISK_ID', 'DEV_DISK_OFFSET', 'DEV_DISK_BUFFER',
//  'DEV_DISK_OPERATION_WRITE', 'DEV_DISK_START_OPERATION', 'DEV_DISK_STATUS'
void ide_write(u_int diskno, u_int secno, void *src, u_int nsecs) {
	u_int begin = secno * BY2SECT;
	u_int end = begin + nsecs * BY2SECT;

	for (u_int off = 0; begin + off < end; off += BY2SECT) {
		uint32_t temp = diskno;
		/* Exercise 5.3: Your code here. (2/2) */
		uint32_t status = 0;
		uint32_t cur_offset = begin + off;
		uint32_t write_flag = DEV_DISK_OPERATION_WRITE;
		//disno
      	panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_ID, sizeof(temp)));
		//disk offset
      	panic_on(syscall_write_dev(&cur_offset, DEV_DISK_ADDRESS + DEV_DISK_OFFSET, sizeof(cur_offset)));
		//buffer
     	panic_on(syscall_write_dev(src + off, DEV_DISK_ADDRESS + DEV_DISK_BUFFER, BY2SECT));
		//1 stands for write
      	panic_on(syscall_write_dev(&write_flag, DEV_DISK_ADDRESS + DEV_DISK_START_OPERATION, sizeof(write_flag)));
		//get return value
      	panic_on(syscall_read_dev(&status, DEV_DISK_ADDRESS + DEV_DISK_STATUS, sizeof(status)));
    	if (status == 0) {
      		user_panic("disk read error 6\n");
    	}
	}
}
