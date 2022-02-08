#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(){
	char *args[]={"sleep","10",NULL};
	pid_t p=fork();
	if(p==0){
		execvp(args[0],args);
	}
	else{
		while(1){
			printf("%d\n",waitpid(p,NULL,1));
			sleep(1);
		}		
	}
}
