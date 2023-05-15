/*
 * operations on IDE disk.
 */

#include "serv.h"
#include <drivers/dev_disk.h>
#include <lib.h>
#include <mmu.h>
#define NONE 0xFFFFFFFF
char noThings[512] = {};
int map[32];
int erase[32];
int writable[32];
void ssd_init() {
	for(int i = 0; i < 32; i++) map[i] = NONE;
	for(int i = 0; i < 32; i++) erase[i] = 0;
	for(int i = 0; i < 32; i++) writable[i] = 1;
}

int ssd_read(u_int logic_no, void *dst) {
	if(map[logic_no] == NONE) {
		return -1;
	}
	ide_read(0, map[logic_no], dst, 1);
	return 0;
}

void ssd_write(u_int logic_no, void *src) {
	if(map[logic_no] != NONE) {
		ssd_erase(logic_no);
		map[logic_no] = NONE;
	}
	u_int phy_no = getPhyNo();
	map[logic_no] = phy_no;
	writable[phy_no] = 0;
	ide_write(0, map[logic_no], src, 1);
}

void ssd_erase(u_int logic_no) {
	int phy_no = map[logic_no];
	if(phy_no == NONE) {
		return -1;
	}
	ide_write(0, phy_no, noThings, 1);
	erase[phy_no]++;
	writable[phy_no] = 1;
	map[logic_no] = NONE;
}

int getPhyNo() {
	u_int times = 100000;
	int ret = -1;
	for (int i = 0; i < 32; i++) {
		if(erase[i] < times && writable[i]) {
			ret = i;
			times = erase[i];
		}
	}
	if(times >= 5) {
		u_int _times = 100000;
		int _ret = -1;
		for (int i = 0; i < 32; i++) {
			if(erase[i] < times && !writable[i]) {
				_ret = i;
				_times = erase[i];
			}
		}
		char *src[512];
		ide_read(0, _ret, src, 1);
		ide_write(0, ret, src, 1);
		writable[ret] = 0;
		int lo_no = -1;
		for(int i = 0; i < 32; i++) if(map[i] == _ret) lo_no = i;
		map[lo_no] = ret;
		ide_write(0, _ret, noThings, 1);
		writable[_ret] = 1;
		return _ret;
	}
	return ret;
}
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
		/* Exercise 5.3: Your code here. (1/2) */
		//diskno
		uint32_t temp = diskno;
      	panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_ID, sizeof(temp)));
		//disk offset
		temp = begin + off;
      	panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_OFFSET, sizeof(temp)));
		//0 stands for read
		temp = DEV_DISK_OPERATION_READ;
      	panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_START_OPERATION, sizeof(temp)));
		//get return value

      	panic_on(syscall_read_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_STATUS, sizeof(temp)));
    	if (temp == 0) {
      		user_panic("return value equals 0\n");
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
		/* Exercise 5.3: Your code here. (2/2) */
		//disno
		uint32_t temp = diskno;
      	panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_ID, sizeof(temp)));
		//disk offset
		temp = begin + off;
      	panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_OFFSET, sizeof(temp)));
		//buffer

     	panic_on(syscall_write_dev(src + off, DEV_DISK_ADDRESS + DEV_DISK_BUFFER, BY2SECT));
		//1 stands for write
		temp = DEV_DISK_OPERATION_WRITE;
      	panic_on(syscall_write_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_START_OPERATION, sizeof(temp)));
		//get return value
		
      	panic_on(syscall_read_dev(&temp, DEV_DISK_ADDRESS + DEV_DISK_STATUS, sizeof(temp)));
    	if (temp == 0) {
      		user_panic("disk read error 6\n");
    	}
	}
}
