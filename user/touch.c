#include <fs.h>
#include <lib.h>

void usage(void) {
	debugf("touch: 缺少要操作的文件\n");
	exit();
}

int main(int argc, char **argv) {
	int r;
	if (argc == 1) {
		usage();
	} else {
		for (int i = 1; i < argc; i++) {
			if ((r = create(argv[i], O_CREAT | FTYPE_REG)) > 0) {
				printf("touch: 无法创建文件 \"%s\": 文件已存在\n", argv[i]);
			}
			if (r < 0) {
				printf("touch: 无法创建文件 \"%s\": 未知错误\n", argv[i]);
			}
			return r;
		}
	}
	return 0;
}
