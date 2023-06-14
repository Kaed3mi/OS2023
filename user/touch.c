#include <fs.h>
#include <lib.h>

void usage(void) {
	debugf("touch: 缺少要操作的文件\n");
	exit();
}

int main(int argc, char **argv) {
	if (argc == 1) {
		usage();
	} else {
		for (int i = 1; i < argc; i++) {
			touch(argv[i]);
		}
	}
	return 0;
}
