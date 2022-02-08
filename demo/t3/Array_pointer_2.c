#include <stdio.h>

void test(int *a){
	int x;
	while((x = *a++) != -1) {
		printf("%d\n",x);
	}
}

void test_char(char **s){

	char *string = NULL;
	while((string = *s++) != NULL){
		printf("%s\n",string);
	}
}

int main(void) {

	int arr[] = {1,2,3,4,5,6,-1};
	test(arr);

	char *s[] = {"hello","world","ni","hao",NULL};
	test_char(s);

	return 0;
}