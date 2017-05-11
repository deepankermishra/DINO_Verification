#include <stdio.h>

void __dino_task_boundary( ... ){
	__asm__("nop");
}

int foo(int n,int a,int b,int c){
__dino_task_boundary(&n,&a,&b,&c);
	int ans=0;
	switch(n){
		case 1 :
__dino_task_boundary(&a);	//requires ans for correctness
			ans+=a;
			break;
		case 2 :
__dino_task_boundary(&b,&ans);
			ans+=b;
			break;
		case 3 :
__dino_task_boundary(&c,&ans);
			ans+=c;
			break;
		default :
			break;
	}
	return ans;
}

int main(int argc,char ** argv){
	__dino_task_boundary(&argc);
	foo(1,1,2,3);
	return argc;
}
