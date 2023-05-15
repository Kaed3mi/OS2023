#include <lib.h>

int main() {
	u_int us = 1000 * 1000;
	u_int before = get_time(&us);
	//usleep(us);
	u_int after = get_time(&us);
	debugf("%d:%d\n", before, us);
	debugf("my : %d : %d\n", us, us / 1000000);
	return 0;
}
