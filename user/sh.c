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
			if ((*rightpipe = fork()) == 0) {  // 子进程执‘|’左侧的cmd
				return argc;
			} else {
				// 父进程执行‘|’右侧的cmd
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

	int child = spawn(argv[0], argv);
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
		// case '\b':
		case 0x7f:
			if (i <= 0) {
				break;
			} else {
				i--;
			}
			for (int j = i + 1; j <= len - 1; j++) {
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
				case 'A':
					MOVEDOWN(1);  // 下移光标
					break;
				case 'B':
					break;
				}
			}
			break;

		case '\r':
		case '\n':
			buf[i] = 0;
			return;

		default:
			buf[len + 1] = 0;
			for (int j = len; j >= i + 1; j--) {
				buf[j] = buf[j - 1];
			}
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
			printf("\n$ ");
		}
		readline(buf, sizeof buf);

		if (buf[0] == '#') {
			continue;
		}
		if (echocmds) {
			printf("# %s\n", buf);
		}
		if ((r = fork()) < 0) {
			user_panic("fork: %d", r);
		}
		if (r == 0) {
			runcmd(buf);
			exit();
		} else {
			wait(r);
		}
	}
	return 0;
}
