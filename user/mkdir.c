#include <fs.h>
#include <lib.h>

void usage(void) {
	debugf("mkdir: 缺少操作对象\n");
	exit();
}

int main(int argc, char **argv) {
	int r;
	if (argc == 1) {
		usage();
	} else {
		for (int i = 1; i < argc; i++) {
			if ((r = create(argv[i], O_CREAT | FTYPE_DIR)) > 0) {
				printf("mkdir: 无法创建目录 \"%s\": 目录已存在\n", argv[i]);
			}
			if (r < 0) {
				printf("mkdir: 无法创建目录 \"%s\": 未知错误\n", argv[i]);
			}
			return r;
		}
	}
	return 0;
}
