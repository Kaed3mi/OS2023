#include <lib.h>

int main() {
	u_int us = 1000 * 1000;
	u_int now_time, entry_time = get_time(NULL) % 100;
	debugf("entry_time : %d\n", entry_time);
	while (1) {
		now_time = get_time(NULL) % 100;
		if(now_time * 1000000 >= entry_time * 1000000 + us) {
			break;
		}
	}
	debugf("now_time : %d\n", now_time);
	return 0;
}
