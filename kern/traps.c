#include <env.h>
#include <pmap.h>
#include <printk.h>
#include <trap.h>

extern void handle_int(void);
extern void handle_tlb(void);
extern void handle_sys(void);
extern void handle_mod(void);
extern void handle_reserved(void);
extern void handle_ov(void);

void (*exception_handlers[32])(void) = {
    [0 ... 31] = handle_reserved,
    [0] = handle_int,
    [2 ... 3] = handle_tlb,
    [12] = handle_ov,
#if !defined(LAB) || LAB >= 4
    [1] = handle_mod,
    [8] = handle_sys,
#endif
};

/* Overview:
 *   The fallback handler when an unknown exception code is encountered.
 *   'genex.S' wraps this function in 'handle_reserved'.
 */
void do_reserved(struct Trapframe *tf) {
	print_tf(tf);
	panic("Unknown ExcCode %2d", (tf->cp0_cause >> 2) & 0x1f);
}

void do_ov(struct Trapframe *tf) {
	u_long va = tf->cp0_epc;
	Pte *pte;
	struct Page *pp = page_lookup(curenv->env_pgdir, va, &pte);
	u_long *kva = (u_long *)((KADDR(page2pa(pp))) | (va & 0xfff));
	u_long add = 0x00000020, sub = 0x00000022, addi = 0x20000000;
	if((*kva & 0xfc0007ff) == add){
		*kva = *kva | 0x1;
		printk("add ov handled\n");
	} else if((*kva & 0xfc0007ff) == sub){
		*kva = *kva | 0x1;
		printk("sub ov handled\n");
	} else if((*kva & 0xfc000000) == addi){
		u_long t_reg = (*kva & 0x001f0000) >> 16;
		u_long s_reg = (*kva & 0x03e00000) >> 21;
		u_long imm   = *kva & 0x0000ffff;
		tf->regs[t_reg] = tf->regs[s_reg]/2 + imm/2;
		tf->cp0_epc += 4;
		printk("addi ov handled\n");
	}
	curenv->env_ov_cnt++;
}
