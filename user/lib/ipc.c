// User-level IPC library routines

#include <env.h>
#include <lib.h>
#include <mmu.h>
#define RTC_ADDR 0x15000000
#define RTC_ALL_OFF 0x0010
#define RTC_US_OFF 0X0020

u_int get_time(u_int *us) {
	u_int temp = 0;
	syscall_write_dev(&temp, RTC_ADDR, sizeof(temp));
	syscall_read_dev(&temp, RTC_ADDR + RTC_ALL_OFF, sizeof(temp));
	syscall_read_dev(us, RTC_ADDR + RTC_US_OFF, sizeof(us));
	return temp;
}


void usleep(u_int us) {
	u_int temp;
	u_int entry_time = get_time(&temp);
	while (1) {
		u_int now_time = get_time(&temp);
		//debugf("us = %d, s = %d\n", us, us * 0.000001);
		if(now_time >= entry_time + (us / 1000000)) {
			return;
		} else {
			syscall_yield();
		}
	}
}


// Send val to whom.  This function keeps trying until
// it succeeds.  It should panic() on any error other than
// -E_IPC_NOT_RECV.
//
// Hint: use syscall_yield() to be CPU-friendly.
void ipc_send(u_int whom, u_int val, const void *srcva, u_int perm) {
	int r;
	while ((r = syscall_ipc_try_send(whom, val, srcva, perm)) == -E_IPC_NOT_RECV) {
		syscall_yield();
	}
	user_assert(r == 0);
}

// Receive a value.  Return the value and store the caller's envid
// in *whom.
//
// Hint: use env to discover the value and who sent it.
u_int ipc_recv(u_int *whom, void *dstva, u_int *perm) {
	int r = syscall_ipc_recv(dstva);
	if (r != 0) {
		user_panic("syscall_ipc_recv err: %d", r);
	}

	if (whom) {
		*whom = env->env_ipc_from;
	}

	if (perm) {
		*perm = env->env_ipc_perm;
	}

	return env->env_ipc_value;
}
