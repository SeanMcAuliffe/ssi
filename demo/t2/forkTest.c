#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
	int a=0;
	printf("initial: %d\n",a);
	pid_t p=fork();
	if(p==0){
		printf("C: address of a: %p\n",(void*)&a);
		a=1;
		printf("C: address of modified a: %p\n",(void*)&a);
		printf("child: %d\n",a);
	}
	else{
		printf("   P: address of a: %p\n",(void*)&a);
		wait(NULL);
		printf("   parent: %d\n",a);
	}
	return 0;
}
