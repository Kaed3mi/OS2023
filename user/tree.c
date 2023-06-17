#include <args.h>
#include <lib.h>

int directory = 0;
int dircCount = 0;
int fileCount = 0;
int linked[32];

void printFile(char *name, int depth, int isEnd, int isDir) {
	for (int i = 0; i < depth; i++) {
		if (linked[i]) {
			printf("│   ");
		} else {
			printf("    ");
		}
	}

	if (isEnd == 0) {
		printf("├── ");
	} else {
		printf("└── ");
	}

	if (isDir == 1) {
		printf("\033[0;34m%s\033[0m\n", name);
	} else {
		printf("%s\n", name);
	}
}

void dfsDir(char *path, int depth) {
	int fdnum, size, va;
	struct Fd *fd;
	struct File f;

	if ((fdnum = open(path, O_RDONLY)) < 0) {
		user_panic("open %s: %d", path, fdnum);
	}
	fd = (struct Fd *)num2fd(fdnum);
	f = ((struct Filefd *)fd)->f_file;
	if (f.f_type == FTYPE_REG) {
		close(fdnum);
		fileCount++;
		close(fdnum);
		return;
	} else {
		dircCount++;
	}

	size = f.f_size;
	va = fd2data(fd);
	for (int i = 0; i < size; i += BY2FILE) {
		struct File *file = (struct File *)(va + i);
		if (!file->f_name[0]) {
			break;
		}
		char nextPath[256];
		const char *dirName = file->f_name;
		int pathLen = strlen(path);
		int nameLen = strlen(dirName);
		strcpy(nextPath, path);
		if (path[pathLen - 1] != '/') {
			nextPath[pathLen++] = '/';
		}
		nextPath[pathLen] = 0;
		strcpy(nextPath + pathLen, dirName);
		int isEnd = (i == size || (file + 1)->f_name[0] == 0) ? 1 : 0;
		if (!isEnd) {
			linked[depth] = 1;
		}
		printFile(dirName, depth, isEnd, file->f_type == FTYPE_DIR);
		dfsDir(nextPath, depth + 1);
		linked[depth] = 0;
	}
}

void tree(char *path) {
	int r;
	struct Stat st;

	if ((r = stat(path, &st)) < 0) {
		user_panic("stat %s: %d", path, r);
	}

	if (!st.st_isdir) {
		printf("%s  [error opening dir]\n", path);
		printf("\n0 directories, 0 files\n");
		exit();
	}

	printf("%s\n", path);
	linked[0] = 1;
	dfsDir(path, 0);
	printf("\n%d directories, %d files\n", dircCount, fileCount);
}

void usage(void) {
	debugf("usage: sh [-dix] [command-file]\n");
	exit();
}

int main(int argc, char **argv) {
	ARGBEGIN {
	case 'd':
		directory = 1;
		break;
	default:
		usage();
		break;
	}
	ARGEND
	if (argc == 0) {
		tree("./");
	} else {
		tree(argv[0]);
	}

	exit();
}
