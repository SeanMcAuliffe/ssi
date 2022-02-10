#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include "linked_list.h"

#define MAX_USER_NAME 255
#define MAX_HOSTNAME_SIZE 255

/* Linked list to store background process information */
bg_list_t* bg_proc_list = NULL;

/* Buffer for malloc()'ed string from readline library */
char* readline_buffer = NULL;

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
    char* rc_ptr = getcwd(current_directory, MAX_INPUT_SIZE);
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
        strncpy(user_login, "default\0", 8); //TODO remove this for linux server
        //exit(1);
    }
}

/* Attempt to parse user input. Returns the number of cmd parts
 * which were tokenized. 0 if no input.
 *
 * Per GNU Readline library documentation caller to readline()
 * must free the returned char* after use. This is done after
 * input is sent to execute_user_cmd().
 * */
int parse_user_input(char** user_input, char* prompt) {
    int cmd_parts = 0;
    readline_buffer = readline(prompt);
    if (strlen(readline_buffer) == 0) {
        free(readline_buffer);
        return cmd_parts;
    }
    if (strlen(readline_buffer) > MAX_INPUT_SIZE) {
        printf("Entered command is long than maximum allowed size.\n");
        free(readline_buffer);
        return 0;
    }
    char* temp = strtok(readline_buffer, " \n");
    for (int i = 0; i < MAX_CMD_NUMBER; i++) {
        user_input[i] = temp;
        cmd_parts++;
        temp = strtok(NULL, " \n");
        if (temp == NULL) {
            break;
        }
    }
    return cmd_parts;
}

void add_background_task(char** cmd, int num_args) {
    char* argv[MAX_CMD_NUMBER];
    // Create new argv without 'bg' keyword in it
    for (int i = 0; i < num_args - 1; i++) {
        argv[i] = cmd[i+1];
    }

    pid_t p = fork();
    if (p == -1) {
        printf("When creating background process, fork() returned error code: -1\n");
        return;
    }
    if (p == 0) {
        exit(1);
        // Child process, execvp;
    } else {
        // If list is not yet instantiated, create it
        if (bg_proc_list == NULL) {
            bg_proc_list = create_list(create_node(p, argv, --num_args));
        } else {
            // Otherwise, just append to it
            list_append(bg_proc_list, create_node(p, argv, --num_args));
        }
    }

}

void add_blocking_child(char** argv) {
    pid_t p = fork();
    if (p == -1) {
        printf("Error occurred in fork() while executing cmd: '%s'\n", argv[0]);
    }
    if (p == 0) {
        /* Attempt to execute use input in child proc */
        int rc = execvp(argv[0], argv);
        if (rc == -1) {
            printf("Error occurred while executing cmd: '%s'\n", argv[0]);
            exit(1);
        }
    } else {
        /* Parent process should wait for the child which was just created */
        waitpid(p, NULL, WUNTRACED | WCONTINUED);
    }
}

/* Executes user cmd, returns bool indicating if program should continue or terminate
 * true = continue, false = terminate. */
bool execute_user_cmd(char** cmds, int num_read) {
    /* Return code for system calls */
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
        // get user home directory
        // TODO this may need to be changed to ENV var method
        struct passwd* pw = getpwuid(getuid());
        if (num_read < 2) { // User typed just 'cd' -> go home directory
            rc = chdir(pw->pw_dir);
        } else if (!strcmp(cmds[1], "~\0")) { // cd ~ -> go home
            rc = chdir(pw->pw_dir);
        } else { // Go to user specified path
            rc = chdir(cmds[1]);
        }
        if (rc != 0) {
            printf("Error: cd returned status code: %d\n", rc);
        }
        free(readline_buffer);
        return true;
    }

    /* 'bg' keyword -> execute cmd in background */
    if (!strcmp(cmds[0], "bg\0")) {
        add_background_task(cmds, num_read);
    } else {
        /* Generic Commands (not to be executed in background) */
        add_blocking_child(cmds);
    }

    /* Free char* malloc()'ed by readline() */
    free(readline_buffer);
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
    char current_directory[MAX_INPUT_SIZE] = {0};

    /* Input buffer, array of char pointers */
    char* user_input[MAX_CMD_NUMBER] = {0};

    /* Prompt to be displayed by readline() */
    char prompt[MAX_INPUT_SIZE];
    char prompt_format[MAX_INPUT_SIZE] = "%s@%s: %s > ";

    /* Initialize Prompt */
    update_login(user_login);
    update_hostname(host_name);
    update_cwd(current_directory);

    bool ongoing = true;
    int num_read = 0;

    while (ongoing) {
        // Update information and reformat prompt
        update_cwd(current_directory);
        sprintf(prompt, prompt_format, user_login, host_name, current_directory);

        // Parse user input
        num_read = parse_user_input(user_input, prompt);
        if (num_read) {
            ongoing = execute_user_cmd(user_input, num_read);
            clear_user_input(user_input, num_read);
        }
    }

    if (bg_proc_list != NULL) {
        process_print(bg_proc_list);
    }

    // Teardown and exit gracefully
    if (bg_proc_list != NULL) {
        destroy_list(bg_proc_list);
    }

    return 0;
}
