#include <stdio.h>

void __dino_task_boundary( ... ){
	__asm__("nop");
}

int foo(int n,int a,int b,int c){
__dino_task_boundary(&n,&a,&b,&c);
	int ans=0;
	if(a>1){
__dino_task_boundary(&n,&b,&c,&ans);
		ans+= n;
	}
	if(b>1){
__dino_task_boundary(&n,&ans);	//requires c for correctness
		ans+= n*2;
	}
	if(c>1){

__dino_task_boundary(&n,&ans);
		ans+= n*3;
	}
	return ans;
}

int main(int argc,char ** argv){
	__dino_task_boundary(&argc);
	foo(10,1,2,3);
	return argc;
}
