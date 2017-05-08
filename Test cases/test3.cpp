#include <stdio.h>

void __dino_task_boundary(...){
	__asm__("nop");
}

int foo(int n , int m) { 
	int sum = 0;int c0;
	int *good;
	good = &n;
	if(sum<20){
		sum += *good;
	}
	while(*good > 0){
		sum+=3;
		(*good)--;
		 __dino_task_boundary(&good,&sum);
	}
	sum++;
	__dino_task_boundary(&sum);
  return sum;
}

int main(int argc , char** argv){
	foo(2,3);
	// che("sum,c0");
	return 0;
}
