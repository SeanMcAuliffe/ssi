#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
	char line[1024];
	char* args[1024];
	fgets(line,1024,stdin);
	args[0]=strtok(line," \n");//split string by space or '\n' (IMPORTANT) there will be error without '\n'
    printf("Output 1: %s, %s \n",args[0],args[1]);
    int i=0;
    while(args[i]!=NULL){//make sure that args has a NULL pointer at the end
        args[i+1]=strtok(NULL," \n");
        i++;
        }
    for(int j=0; j<i; j++)
    {
        printf("Output 2: %s \n",args[j]);
    }
	}