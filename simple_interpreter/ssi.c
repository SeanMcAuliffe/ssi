#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <poll.h>
#include <readline/readline.h>
#include "linked_list.h"

#define MAX_USERNAME_SIZE 255
#define MAX_HOSTNAME_SIZE 255
#define MAX_INPUT_SIZE 255
#define MAX_PATH_LENGTH 1024


void clear_buffer(char* buffer) {
    for (int i = 0; i < MAX_INPUT_SIZE; i++) {
        buffer[i] = 0;
    }
}

bool is_ongoing(const char* buffer, ssize_t len) {

    char* literal = "exit\0";
    bool result = true;

    if (strcmp(buffer, "\n") == 0){
        return true;
    }

    for (int i = 0; i < len - 1; i++) {
        if (buffer[i] != literal[i]) {
            result = false;
        }
    }

    return !result;
}


int main() {

    /* Input buffers */
    char* user_login = NULL;
    char* user_input_buffer = NULL;
    char host_name[MAX_HOSTNAME_SIZE] = {0};
    char current_directory[MAX_PATH_LENGTH] = {0};
    bool ongoing = true;


    /* return code */
    int rc = -1;
    ssize_t num_read = 0;
    size_t len = 0;

    /* Get info for prompt */
    rc = gethostname(host_name, sizeof(host_name));
    if (rc != 0) {
        printf("Error in gethostname()\n");
    }

    user_login = getlogin();
    if (user_login == NULL) {
        user_login = "default";
    }

    getcwd(current_directory, sizeof(current_directory));

    printf("%s@%s: %s > \n", user_login, host_name, current_directory);

    while (ongoing) {
        num_read = getline(&user_input_buffer, &len, stdin);
        ongoing = is_ongoing(user_input_buffer, num_read);
        clear_buffer(user_input_buffer);
        getcwd(current_directory, sizeof(current_directory));
        printf("%s@%s: %s > ", user_login, host_name, current_directory);
    }

    printf("\n");
    return 0;
}
