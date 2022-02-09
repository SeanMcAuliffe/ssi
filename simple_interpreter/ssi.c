#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#define MAX_SIZE 80

int main() {
    char* user_login = NULL;
    char host_name[MAX_SIZE];
    user_login = getlogin();
    if (user_login == NULL){
        printf("No login detected. Setting default.\n");
        user_login = "default";
    }
    gethostname(host_name, MAX_SIZE*sizeof(char));
    printf("%s@%s >> \n", user_login, host_name);
    return 0;
}
