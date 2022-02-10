#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "linked_list.h"
#include <pwd.h>

#define MAX_USER_NAME 255
#define MAX_HOSTNAME_SIZE 255
#define MAX_CMD_NUMBER 64
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
        //printf("Error: get_login_r() returned %d\n", rc);
        strncpy(user_login, "default\0", 8);
        //exit(1);
    }
}

/* Attempt to parse user input. Returns the number of cmd parts
 * which were tokenized. 0 if no input. */
int parse_user_input(char** user_input, char* prompt) {
    int count = 0;
    char* buffer = readline(prompt);
    if (strlen(buffer) == 0){
        return count;
    }
    char* temp = strtok(buffer, " ");
    for (int i = 0; i < MAX_CMD_NUMBER; i++) {
        user_input[i] = temp;
        count++;
        temp = strtok(NULL, " ");
        if (temp == NULL) {
            break;
        }
    }
    return count;
}

/* Executes user cmd, returns bool indicating if program should continue or terminate
 * true = continue, false = terminate. */
bool execute_user_cmd(char** cmds, int num_read) {
    /* Return code for system call */
    int rc = -1;

    /* Special cases */

    // No command to execute
    if (num_read == 0) {
        return true;
    }

    // 'Exit' keyword
    if (!strcmp(cmds[0], "exit\0")) {
        return false;
    }

    // 'cd' keyword, implement using chdir()
    if(!strcmp(cmds[0], "cd\0")) {
        struct passwd* pw = getpwuid(getuid()); // get user home directory
        if (num_read < 2) { // User typed just 'cd' -> go home directory
            rc = chdir(pw->pw_dir);
        } else if (!strcmp(cmds[1], "~\0")) {
            rc = chdir(pw->pw_dir);
        } else {
            rc = chdir(cmds[1]);
        }
        if (rc != 0) {
            printf("Error: cd returned status code of %d\n", rc);
        }
        return true;
    }

    /* Generic Commands (not to be executed in background) */

    pid_t p = fork();
    if (p == 0) { // Child process
//        printf("In child process.\n");
        int rc = execvp(cmds[0], cmds);
        //printf("rc: %d\n", rc);
        if (rc == -1) {
            printf("Error occurred while executing %s cmd.\n", cmds[0]);
            exit(1);
        }
    } else { // Parent process
       // printf("Parent waiting\n");
        waitpid(p, NULL, WUNTRACED | WCONTINUED);
       // printf("Child returned to parent.\n");
    }
    return true;
}

/* Removes old commands from the input buffer */
void clear_user_input(char** buffer, int n) {
    for (int i = 0; i < n; i++) {
        buffer[i] = NULL;
    }
}

int main() {

    /* Prompt information */
    char user_login[MAX_USER_NAME] = {0};
    char host_name[MAX_HOSTNAME_SIZE] = {0};
    char current_directory[MAX_PATH_LENGTH] = {0};

    /* Input buffer, array of char pointers */
    char* user_input[MAX_CMD_NUMBER] = {0};

    /* Prompt to be displayed by readline() */
    char prompt[1024];
    char prompt_format[1014] = "%s@%s: %s > ";

    /* Initialize Prompt */
    update_login(user_login);
    update_hostname(host_name);
    update_cwd(current_directory);

    bool ongoing = true;
    int num_read = 0;

    while (ongoing) {
        // Update information and reformat prompt
        // TODO: More efficient to do this only when user enters cd command
        update_cwd(current_directory);
        sprintf(prompt, prompt_format, user_login, host_name, current_directory);

        // Parse user input
        num_read = parse_user_input(user_input, prompt);
        if (num_read) {
            ongoing = execute_user_cmd(user_input, num_read);
            clear_user_input(user_input, num_read);
        }
    }

    // Teardown and exit gracefully
    if (bg_proc_list != NULL) {
        destroy_list(bg_proc_list);
        num_read++;
    }

    return 0;
}
