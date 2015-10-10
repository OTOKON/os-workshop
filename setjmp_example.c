#include <setjmp.h>
#include <stdio.h>


static jmp_buf buf1;

int bar(){
	longjmp(buf1, 1);
	printf("\nbar func");
	return 1;
}

int foo(int x){
	int z = x;
	if(!setjmp(buf1)){
		printf("\nsetjmp - 0");
		bar();
	}
	printf("\nsetjmp - 1\n");
	return z;
}

int main(){
	foo(123);
	return 0;
}
