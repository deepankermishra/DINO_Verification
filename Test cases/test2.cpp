#include <stdio.h>

void __dino_task_boundary( ... ){
	__asm__("nop");
}

int fibonacci(int n){
    if(n == 0)
    	return 0;
    else if(n == 1)
    	return 1;
    else
    {
    	int ans = fibonacci(n-1)+fibonacci(n-2);
		__dino_task_boundary(&ans);
    	return ans;
    }
}

int main(int argc,char ** argv){
	__dino_task_boundary(&argc);
	fibonacci(10);
	return argc;
}