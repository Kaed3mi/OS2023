#include <args.h>
#include <lib.h>

int main(int argc, char **argv) {
	// printf("builtout cd\n");
	int r;
	struct Stat st = {0};
	char cur[1024] = {0};

	if (argc == 1) {
		exit();
	} else if (argv[1][0] != '/') {
		char *p = argv[1];
		if (argv[1][0] == '.') {
			p += 2;
		}
		getchcwd(cur);
		int curlen = strlen(cur);
		int len = strlen(p);
		if (curlen == 1) {	// cur: '/'
			strcpy(cur + 1, p);
		} else {  // cur: '/a'
			cur[curlen] = '/';
			strcpy(cur + curlen + 1, p);
			cur[curlen + 1 + len] = '\0';
		}
	} else {
		strcpy(cur, argv[1]);
	}
	// debugf("%s\n", cur);
	if ((r = stat(cur, &st)) < 0) {
		printf("cd: %s: 没有那个文件或目录\n", argv[1]);
		exit();
	}
	if (!st.st_isdir) {
		printf("cd: %s: 不是目录\n", argv[1]);
		exit();
	}
	if ((r = chshdir(cur)) < 0) {
		printf("cd: %s: 切换目录异常\n", argv[1]);
	}
	exit();
}
