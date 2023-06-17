#include <args.h>
#include <lib.h>

int main(int argc, char **argv) {
	// printf("argc = %d\n", argc);
	if (argc == 1) {
		int m = 0, n = 0;
		while (n < 4) {
			if (m > 99999999) {
				debugf("\ntestbgd: now n = %d", n);
				n++;
			} else {
				m++;
			}
		}
		debugf("\n");
	} else {
		debugf("testbgd: 格式错误\n");
		exit();
	}
	exit();
}