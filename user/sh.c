#include <args.h>
#include <lib.h>

#define WHITESPACE " \t\r\n"
#define SYMBOLS "<|>&;()"
#define MOVEUP(n) printf("\033[%dA", (n))
#define MOVEDOWN(n) printf("\033[%dB", (n))
#define MOVELEFT(n) printf("\033[%dD", (n))
#define MOVERIGHT(n) printf("\033[%dC", (n))
/* Overview:
 *   Parse the next token from the string at s.
 *
 * Post-Condition:
 *   Set '*p1' to the beginning of the token and '*p2' to just past the token.
 *   Return:
 *     - 0 if the end of string is reached.
 *     - '<' for < (stdin redirection).
 *     - '>' for > (stdout redirection).
 *     - '|' for | (pipe).
 *     - 'w' for a word (command, argument, or file name).
 *
 *   The buffer is modified to turn the spaces after words into zero bytes ('\0'), so that the
 *   returned token is a null-terminated string.
 */
int _gettoken(char *s, char **p1, char **p2) {
	*p1 = 0;
	*p2 = 0;
	if (s == 0) {
		return 0;
	}

	while (strchr(WHITESPACE, *s)) {
		*s++ = 0;
	}
	if (*s == 0) {
		return 0;
	}

	if (strchr(SYMBOLS, *s)) {
		int t = *s;
		*p1 = s;
		*s++ = 0;
		*p2 = s;
		return t;
	}

	if (*s == '\"') {
		*p1 = ++s;
		while (*s && *s != '\"') {
			s++;
		}
		*p2 = s;
		*s++ = 0;
		return 'w';
	}

	*p1 = s;
	while (*s && !strchr(WHITESPACE SYMBOLS, *s)) {
		s++;
	}
	*p2 = s;
	return 'w';
}

int gettoken(char *s, char **p1) {
	static int c, nc;
	static char *np1, *np2;

	if (s) {
		nc = _gettoken(s, &np1, &np2);
		return 0;
	}
	c = nc;
	*p1 = np1;
	nc = _gettoken(np2, &np1, &np2);
	return c;
}

#define MAXARGS 128

int parsecmd(char **argv, int *rightpipe) {
	int argc = 0;
	while (1) {
		char *t;
		int fd, r;
		int c = gettoken(0, &t);
		switch (c) {
		case 0:
			return argc;
		case 'w':
			if (argc >= MAXARGS) {
				debugf("too many arguments\n");
				exit();
			}
			argv[argc++] = t;
			break;
		case '<':
			if (gettoken(0, &t) != 'w') {
				debugf("syntax error: < not followed by word\n");
				exit();
			}
			// Open 't' for reading, dup it onto fd 0, and then close the original
			// fd.
			/* Exercise 6.5: Your code here. (1/3) */
			if ((r = open(t, O_RDONLY)) < 0) {
				user_panic("user/sh.c:case '<': Exception: opening files!\n");
			}
			fd = r;
			dup(fd, 0);
			close(fd);
			break;
		case '>':
			if (gettoken(0, &t) != 'w') {
				debugf("syntax error: > not followed by word\n");
				exit();
			}
			// Open 't' for writing, dup it onto fd 1, and then close the original
			// fd.
			/* Exercise 6.5: Your code here. (2/3) */
			if ((r = open(t, O_WRONLY)) < 0) {
				user_panic("user/sh.c:case '>': Exception: opening files!\n");
			}
			fd = r;
			dup(fd, 1);
			close(fd);
			break;

		case '|':;
			/*
			 * First, allocate a pipe.
			 * Then fork, set '*rightpipe' to the returned child envid or zero.
			 * The child runs the right side of the pipe:
			 * - dup the read end of the pipe onto 0
			 * - close the read end of the pipe
			 * - close the write end of the pipe
			 * - and 'return parsecmd(argv, rightpipe)' again, to parse the rest of
			 * the command line. The parent runs the left side of the pipe:
			 * - dup the write end of the pipe onto 1
			 * - close the write end of the pipe
			 * - close the read end of the pipe
			 * - and 'return argc', to execute the left of the pipeline.
			 */
			int p[2];
			/* Exercise 6.5: Your code here. (3/3) */
			pipe(p);
			if ((*rightpipe = fork()) == 0) {
				dup(p[0], 0);
				close(p[0]);
				close(p[1]);
				return parsecmd(argv, rightpipe);
			} else {
				dup(p[1], 1);
				close(p[1]);
				close(p[0]);
				return argc;
			}
			break;
		case ';':
			if ((*rightpipe = fork()) == 0) {  // 子进程执‘;’左侧的cmd
				return argc;
			} else {
				// 父进程执行‘;’右侧的cmd
				wait(*rightpipe);
				return parsecmd(argv, rightpipe);
			}
			break;
		case '&':
			if (fork() == 0) {
				return argc;
			} else {
				return parsecmd(argv, rightpipe);
			}
			break;
		}
	}

	return argc;
}

void runcmd(char *s) {
	gettoken(s, 0);

	char *argv[MAXARGS];
	int rightpipe = 0;
	int argc = parsecmd(argv, &rightpipe);
	if (argc == 0) {
		return;
	}
	argv[argc] = 0;

	int child;
	// 判断加了".b"是否可以运行
	if ((child = spawn(argv[0], argv)) < 0) {
		char expanded[1024] = {};
		strcpy(expanded, argv[0]);
		int len = strlen(expanded);
		strcpy(expanded + len, ".b");
		child = spawn(expanded, argv);
	}
	close_all();
	if (child >= 0) {
		wait(child);
	} else {
		debugf("spawn %s: %d\n", argv[0], child);
	}
	if (rightpipe) {
		wait(rightpipe);
	}
	exit();
}

static int hisCnt, hisPos;	// hisPos表示当前指令是第几个历史指令，hisCnt表示一共有多少个历史指令
static int hisBufLen[1024];
static char curCmd[1024];  // 用来缓存当前输入的指令

int cleanCmd(int i, int len) {
	if (i != 0) {
		MOVELEFT(i);
	}
	for (int j = 0; j < len; j++) {
		printf(" ");
	}
	if (len != 0) {
		MOVELEFT(len);
	}
	return 0;
}

int readHisCmd(int hisPos, char *buf) {
	int r, fd, spot = 0;
	char buff[10240];
	fd = open("/.history", O_RDONLY);
	for (int i = 0; i < hisPos; i++) {
		spot += (hisBufLen[i] + 1);	 // 寻找偏移
	}
	readn(fd, buf, spot);
	readn(fd, buf, hisBufLen[hisPos]);
	close(fd);

	buf[hisBufLen[hisPos]] = '\0';
	// debugf("readHisCmd : %s\n", buf);
	return 0;
}

void readline(char *buf, u_int n) {
	int r, len = 0, i = 0;
	char ch;
	// i 表示当前控制台信息指针的下表
	while (len < n) {
		if ((r = read(0, &ch, 1)) != 1) {
			if (r < 0) {
				debugf("read error: %d\n", r);
			}
			exit();
		}
		switch (ch) {
		// Backspace
		case 0x7f:
			if (i <= 0) {
				break;
			}
			for (int j = --i; j < len - 1; j++) {
				buf[j] = buf[j + 1];
			}
			buf[--len] = 0;
			MOVELEFT(i + 1);
			printf("%s ", buf);
			MOVELEFT(len - i + 1);
			break;

		// ← and →
		// \033[D 表示左箭头	\033[C 表示右箭头
		case '\033':
			read(0, &ch, 1);
			if (ch == '[') {
				read(0, &ch, 1);
				switch (ch) {
				// 左箭头 ←
				case 'D':
					if (i > 0) {
						i--;
					} else {
						MOVERIGHT(1);  // 右移光标
					}
					break;
				// 右箭头 →
				case 'C':
					if (i < len) {
						i++;
					} else {
						MOVELEFT(1);  // 左移光标
					}
					break;
				// 上箭头
				case 'A':
					MOVEDOWN(1);  // 下移光标
					// debugf("上: hisPos = %d\n", hisPos);
					if (hisPos > 0) {
						if (hisPos == hisCnt) {
							// 缓存当前输入的指令
							strcpy(curCmd, buf);
							// debugf("\ncurCmd = %s\n", curCmd);
						}
						hisPos--;
						cleanCmd(i, len);
						readHisCmd(hisPos, buf);
						printf("%s", buf);
						i = strlen(buf);
						len = i;  // 重定位光标指针
					}
					break;
				// 下箭头
				case 'B':
					// debugf("下: hisPos = %d\n", hisPos);
					cleanCmd(i, len);
					if (hisPos < hisCnt) {
						hisPos++;
					}
					if (hisPos < hisCnt) {
						readHisCmd(hisPos, buf);
					} else if (hisPos == hisCnt) {
						strcpy(buf, curCmd);
					}
					printf("%s", buf);
					i = strlen(buf);
					len = i;  // 重定位光标指针
					break;
				}
			}
			break;

		case '\r':
		case '\n':

			if (hisCnt == 0) {
				try(touch("/.history"));
			}
			int hisFd;
			hisFd = open("/.history", O_APPEND | O_WRONLY);
			write(hisFd, buf, len);
			write(hisFd, "\n", 1);
			close(hisFd);
			hisBufLen[hisCnt++] = len;
			hisPos = hisCnt;
			// debugf("hisCnt = %d, hisPos = %d\n", hisCnt, hisPos);
			memset(curCmd, 0, sizeof(curCmd));
			return;

		default:
			// 将字符ch插入buf
			for (int j = len; j >= i + 1; j--) {
				buf[j] = buf[j - 1];
			}
			buf[len + 1] = '\0';
			buf[i++] = ch;

			MOVELEFT(i);
			printf("%s", buf);
			if (len + 1 - i != 0) {
				MOVELEFT(len + 1 - i);	// 左移光标
			}
			len++;
			break;
		}
	}

	debugf("line too long\n");
	while ((r = read(0, buf, 1)) == 1 && buf[0] != '\r' && buf[0] != '\n') {
		;
	}
	buf[0] = 0;
}

char buf[1024];

void usage(void) {
	debugf("usage: sh [-dix] [command-file]\n");
	exit();
}

int main(int argc, char **argv) {
	int r;
	int interactive = iscons(0);
	int echocmds = 0;
	char cwd[256] = {};
	printf("\033[2J");	// 清屏
	debugf("\n:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	debugf("::                                                         ::\n");
	debugf("::                     MOS Shell 2023                      ::\n");
	debugf("::                                                         ::\n");
	debugf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
	ARGBEGIN {
	case 'i':
		interactive = 1;
		break;
	case 'x':
		echocmds = 1;
		break;
	default:
		usage();
	}
	ARGEND

	if (argc > 1) {
		usage();
	}
	if (argc == 1) {
		close(0);
		if ((r = open(argv[1], O_RDONLY)) < 0) {
			user_panic("open %s: %d", argv[1], r);
		}
		user_assert(r == 0);
	}
	for (;;) {
		if (interactive) {
			getcwd(cwd);
			printf("\033[5;1;32mmos@21373512\033[0m");
			printf(":");
			printf("\033[5;1;34m~%s \033[0m", cwd);
			printf("\033[5;0;31m[%08x]\033[0m", syscall_getenvid());
			printf(" $ ");
		}
		readline(buf, sizeof buf);

		if (buf[0] == '#') {
			continue;
		}
		if (echocmds) {
			printf("# %s\n", buf);
		}
		int r;
		if ((r = fork()) < 0) {
			user_panic("fork: %d", r);
		}
		if (r == 0) {
			runcmd(buf);
		} else {
			wait(r);
		}
	}
	return 0;
}
