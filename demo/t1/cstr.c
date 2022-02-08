#include <stdio.h>
int main(){
	char str[]="hello world";
	str[5]='\0';
	printf("%s",str);
	return 0;
}
