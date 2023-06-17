#include <lib.h>

void usage(void) {
	debugf("mkdir: 缺少操作对象\n");
	exit();
}

int main(int argc, char **argv) {
	if (argc == 1) {
		usage();
	} else {
		for (int i = 1; i < argc; i++) {
			mkdir(argv[i]);
		}
	}
	exit();
}
