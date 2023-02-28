.PHONY: clean

out: calc case_all
	./calc < case_all > out

# Your code here.

calc:
	gcc -o calc calc.c

clean:
	rm -f out calc casegen case_*

case_add:
	gcc -o casegen casegen.c
	./casegen add 100 > case_add

case_sub:
	gcc -o casegen casegen.c
	./casegen sub 100 > case_sub

case_mul:
	gcc -o casegen casegen.c
	./casegen mul 100 > case_mul

case_div:
	gcc -o casegen casegen.c;
	./casegen div 100 > case_div

case_all: case_add case_sub case_mul case_div
	cat case_add case_sub case_mul case_div > $@


