#include "elf.h"
#include <stdio.h>

/* Overview:
 *   Check whether specified buffer is valid ELF data.
 *
 * Pre-Condition:
 *   The memory within [binary, binary+size) must be valid to read.
 *
 * Post-Condition:
 *   Returns 0 if 'binary' isn't an ELF, otherwise returns 1.
 */
int is_elf_format(const void *binary, size_t size) {
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary;
	return size >= sizeof(Elf32_Ehdr) && ehdr->e_ident[EI_MAG0] == ELFMAG0 &&
	       ehdr->e_ident[EI_MAG1] == ELFMAG1 && ehdr->e_ident[EI_MAG2] == ELFMAG2 &&
	       ehdr->e_ident[EI_MAG3] == ELFMAG3;
}

/* Overview:
 *   Parse the sections from an ELF binary.
 *
 * Pre-Condition:
 *   The memory within [binary, binary+size) must be valid to read.
 *
 * Post-Condition:
 *   Return 0 if success. Otherwise return < 0.
 *   If success, output the address of every section in ELF.
 */

int readelf(const void *binary, size_t size) {
	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)binary;

	// Check whether `binary` is a ELF file.
	if (!is_elf_format(binary, size)) {
		fputs("not an elf file\n", stderr);
		return -1;
	}

	// Get the address of the section table, the number of section headers and the size of a
	// section header.
	// const void *sh_table;
	// Elf32_Half sh_entry_count;
	// Elf32_Half sh_entry_size;
	/* Exercise 1.1: Your code here. (1/2) */
	// sh_table = binary + ehdr->e_shoff;	//表头地址
	// sh_entry_count = ehdr->e_shnum;		//表项的数量
	// sh_entry_size = ehdr->e_shentsize;	//每个表项的大小
	// For each section header, output its index and the section address.
	// The index should start from 0.
	// for (int i = 0; i < sh_entry_count; i++) {
	// 	const Elf32_Shdr *shdr;
	// 	unsigned int addr;
	// 	/* Exercise 1.1: Your code here. (2/2) */
	// 	shdr = (Elf32_Shdr *)(sh_table + i * sh_entry_size);
	// 	addr = shdr->sh_addr;
	// 	printf("%d:0x%x\n", i, addr);
	// }

	//去年课上
	const void *ph_table;
	Elf32_Half ph_entry_count;
	Elf32_Half ph_entry_size;
	/* Exercise 1.1: Your code here. (1/2) */
	ph_table = binary + ehdr->e_phoff;	//表头地址
	ph_entry_count = ehdr->e_phnum;		//表项的数量
	ph_entry_size = ehdr->e_phentsize;	//每个表项的大小
	// For each section header, output its index and the section address.
	// The index should start from 0.
	for(int i = 0; i < ph_entry_count-1; i++){
		const Elf32_Phdr *phdr, *phdr_next;
		unsigned int l_1, r_1, l_2, r_2;
		phdr = (Elf32_Phdr *)(ph_table + i * ph_entry_size);
		phdr_next = (Elf32_Phdr *)(ph_table + (i+1) * ph_entry_size);
		addr = phdr->p_vaddr;
		l_1 = phdr->p_vaddr; r_1 = l_1 + phdr->p_memsz;
		l_2 = phdr_next->p_vaddr; r_2 = l_2 + phdr_next->p_memsz;
		if(ROUNDDOWN(r_1, BY2PG)==ROUNDDOWN(l_2,BY2PG) && l_2 >= r_1) {
			printf("Overlay at page va : 0x%x\n", ROUNDDOWN(r_1,BY2PG));
			return 0;
		} else if(l_2 < r_1){
			printf("Conflict at page va : 0x%x\n", ROUNDDOWN(r_1,BY2PG));
			return 0;
		}
	}

	for (int i = 0; i < ph_entry_count; i++) {
		const Elf32_Phdr *phdr, *phdr_next;
		unsigned int addr;

		phdr = (Elf32_Phdr *)(ph_table + i * ph_entry_size);
		printf("%d:0x%x,0x%x\n", i, phdr->p_filesz, phdr->p_memsz);
	}

	return 0;
}
