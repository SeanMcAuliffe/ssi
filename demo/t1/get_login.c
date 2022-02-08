#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main(){
	uid_t uid;
	gid_t gid;
	uid = getuid();
	printf("User is %s\n", getlogin());
	printf("User IDs: uid=%d,\n", uid);
}