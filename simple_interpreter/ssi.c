#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <readline/readline.h>
#include "linked_list.h"

#define MAX_USER_NAME 255
#define MAX_HOSTNAME_SIZE 255
#define MAX_INPUT_SIZE 1024
#define MAX_PATH_LENGTH 1024

/* Linked list to store background process information */
bg_list_t* bg_proc_list = NULL;

/* Wrapper for gethostname() */
void update_hostname(char* hostname) {
    int rc = gethostname(hostname, MAX_HOSTNAME_SIZE);
    if (rc != 0) {
        printf("Error: gethostname() returned %d\n", rc);
        exit(1);
    }
}

/* Wrapper for getcwd() */
void update_cwd(char* current_directory) {
    char* rc_ptr = getcwd(current_directory, MAX_PATH_LENGTH);
    if (rc_ptr == NULL) {
        printf("Error: getcwd() returned NULL\n");
        exit(1);
    }
}

/* Wrapper for get_login_r() */
void update_login(char* user_login) {
    int rc = getlogin_r(user_login, MAX_USER_NAME);
    if (rc != 0) {
        printf("Error: get_login_r() returned %d\n", rc);
        strncpy(user_login, "default\0", 8);
        //exit(1);
    }
}

/* Attempt to parse user input. If new input is present
 * return true, else false */
bool parse_user_input(char* user_input) {
    user_input = readline((const char*)"");
    printf("%s", user_input);
    return false;
}

int main() {

    /* Prompt information */
    char user_login[MAX_USER_NAME] = {0};
    char host_name[MAX_HOSTNAME_SIZE] = {0};
    char current_directory[MAX_PATH_LENGTH] = {0};

    /* Input buffer */
    char user_input[MAX_INPUT_SIZE] = {0};

    /* Initialize Prompt */
    update_login(user_login);
    update_hostname(host_name);
    update_cwd(current_directory);

    bool ongoing = true;
    bool input_success = false;

    while (ongoing) {
        printf("%s@%s: %s > ", user_login, host_name, current_directory);
        input_success = parse_user_input(user_input);
        update_cwd(current_directory);
        ongoing = false;
    }

    // Teardown and exit gracefully
    if (bg_proc_list != NULL) {
        destroy_list(bg_proc_list);
    }

    return 0;
}
