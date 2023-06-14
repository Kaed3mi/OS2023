#include <args.h>
#include <lib.h>

int main(int argc, char **argv) {
	int r;
	struct Stat st = {0};
	char cur[1024] = {0};
	if (argc == 1) {
		cur[0] = '/';
	} else if (argv[1][0] != '/') {
		char *p = argv[1];
		if (argv[1][0] == '.') {
			p += 2;
		}
		getcwd(cur);
		int len1 = strlen(cur);
		int len2 = strlen(p);
		if (len1 == 1) {  // cur: '/'
			strcpy(cur + 1, p);
		} else {  // cur: '/a'
			cur[len1] = '/';
			strcpy(cur + len1 + 1, p);
			cur[len1 + 1 + len2] = '\0';
		}
	} else {
		strcpy(cur, argv[1]);
	}
	// printf("%s\n", cur);
	if ((r = stat(cur, &st)) < 0) {
		printf("cd: %s: 没有那个文件或目录\n", argv[1]);
		return;
	}
	if (!st.st_isdir) {
		printf("cd: %s: 不是目录\n", argv[1]);
		return;
	}
	if ((r = chshdir(cur)) < 0) {
		printf("cd: %s: 切换目录异常\n", argv[1]);
	}
	return;
}
