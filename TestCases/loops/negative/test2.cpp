#include <stdio.h>

void __dino_task_boundary( ... ){
	__asm__("nop");
}

int foo(int n,int m){
__dino_task_boundary(&n,&m);
    int ans=0;
__dino_task_boundary(&ans,&n,&m);
	for(int i=0;i<m;i+=n){
		ans+=m*i;
		__dino_task_boundary(&ans,&m,&n);//error requires i for correctness
	}
	return ans;
}

int main(int argc,char ** argv){
	__dino_task_boundary(&argc);
	foo(2,10);
return argc;
}
