#include <lib.h>

int main(int argc, char **argv) {
	int fd, r, hisPos = 1;
	char temp[1], ch;
	if ((fd = open("/.history", O_RDONLY)) < 0) {
		user_panic("history: %d", fd);
	}
	ch = temp[0];
	printf(" %4d : ", hisPos);
	while ((r = readn(fd, &temp, 1)) == 1) {
		printf("%c", ch);
		if (ch == '\n') {
			printf(" %4d : ", ++hisPos);
		}
		ch = temp[0];
	}
	printf("\n");
	exit();
}