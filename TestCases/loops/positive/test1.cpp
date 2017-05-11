#include <stdio.h>

void __dino_task_boundary( ... ){
	__asm__("nop");
}

int foo(int n,int m){
__dino_task_boundary(&n,&m);
    int x=0, y=0;

__dino_task_boundary(&x,&y,&n,&m);
	while(n>m){
		x++;
		y = x*2;
		m++;
	}
__dino_task_boundary(&x,&y,&n,&m);
	return x+n+m+y;
}

int main(int argc,char ** argv){
	__dino_task_boundary(&argc);
		foo(10,10);
return argc;
}
