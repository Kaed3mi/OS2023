#include <args.h>
#include <lib.h>

void usage(void) {
	debugf("usage: pwd\n");
	exit();
}

int main(int argc, char **argv) {
	if (argc != 1) {
		usage();
		exit();
	} else {
		char path[128] = {0};
		getcwd(path);
		printf("%s\n", path);
	}

	return 0;
}
