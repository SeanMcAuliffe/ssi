#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(){
	char host_name[1024];
	char *user_name;
	user_name = getlogin();
	gethostname(host_name, 1024);
	printf("%s@%s> \n",user_name,host_name);
}