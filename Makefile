.PHONY: clean

out: calc case_all
	./calc < case_all > out

# Your code here.

calc:
	gcc -o calc calc.c

clean:
	rm -f out calc casegen case_*

case_add:
	gcc -o casegen casegec
	touch case_add
	rm case_add
	./casegen add 100 > ./case_add

case_sub:
	gcc -o casegen casegen.c
	touch case_sub
	rm case_sub
	./casegen sub 100 > ./case_sub

case_mul:
	gcc -o casegen casegen.c
	touch case_mul
	rm case_mul
	./casegen mul 100 > ./case_mul

case_div:
	gcc -o casegen casegen.c
	touch case_div
	rm case_div
	./casegen div 100 > ./case_div

case_all: case_add case_sub case_mul case_div
	touch case_all
	rm case_all
	cat case_add | cat >> case_all
	cat case_sub | cat >> case_all
	cat case_mul | cat >> case_all
	cat case_div | cat >> case_all



