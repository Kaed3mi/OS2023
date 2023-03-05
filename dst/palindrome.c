#include <stdio.h>
#include "string.h"
int isPali(int num);
int main() {
	int n;
	scanf("%d", &n);

	if (isPali(n)) {
		printf("Y\n");
	} else {
		printf("N\n");
	}
	return 0;
}

int isPali(int num) {
	char str[10] = {};
	for(int i = 0;num != 0 ;i++) {
		str[i] = num % 10;
		num = num / 10;
	}
	int len = strlen(str);
	int left = 0, right = len - 1;
	while(left < right) {
		if(str[left] != str[right])
			return 0;
		left++;
		right--;
	}
	return 1;
}
