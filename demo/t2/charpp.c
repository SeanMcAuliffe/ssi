#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
int main(){
	char** s=(char**)malloc(sizeof(char*)*4);
	s[0]="bg";
	s[1]="ls";
	s[2]="-l";
	s[3]=NULL;
	s++;
	execvp(s[0],s);
	return 0;
}
