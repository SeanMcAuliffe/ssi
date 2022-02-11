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
#include <time.h>
#include "linked_list.h"

#define MAX_HOSTNAME_SIZE 1024

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
char* update_login() {
    char* rc = getlogin();
    if (rc == NULL) {
        //printf("Error: get_login() returned NULL\n");
        return "default_user\0";
        //exit(1);
    }
    return rc;
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
        printf("Error: command is longer than maximum allowed size.\n");
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

    /* Create new argv without 'bg' keyword in it */
    char* argv[MAX_CMD_NUMBER];
    for (int i = 0; i < num_args; i++) {
        argv[i] = cmd[i+1];
    }
    pid_t p = fork();
    if (p == -1) {
        printf("While creating background process, fork() returned error code: -1\n");
        return;
    }
    else if (p == 0) {
        int rc = execvp(argv[0], argv);
        if (rc == -1) {
            printf("Error occurred while creating background task.\n");
            exit(1);
        }
    } else {
        /* This small delay, is improper I know.
            But on the linux server it allows the
            prompt to print in a more natural location
            after a bg task has terminated */
        struct timespec t;
        t.tv_sec = 0;
        t.tv_nsec = 100000000;
        nanosleep(&t, &t);
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
    int rc = -1;

    /* Special cases */

    // No command to execute
    if (num_read == 0) {
        return true;
    }

    // 'Exit' keyword
    else if (!strcmp(cmds[0], "exit\0")) {
        return false;
    }

    // 'bglist' keyword
    else if (!strcmp(cmds[0], "bglist\0")) {
        process_print(bg_proc_list);
    }

    // 'cd' keyword, implement using chdir()
    else if(!strcmp(cmds[0], "cd\0")) {
        // get user home directory
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
    }

    /* 'bg' keyword -> execute cmd in background */
    else if (!strcmp(cmds[0], "bg\0")) {
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

/* Checks if any of the previously created bg processes have terminated.
 * Returns true if a child has terminated and been removed from LL. */
bool check_bg_proc_status() {
    if (bg_proc_list == NULL) {
        return false; // no background processes
    }
    if (bg_proc_list->length == 0) {
        return false; // no background processes
    }
    pid_t pid = waitpid(-1, NULL, WNOHANG);
    if (pid == 0) {
        return false; // no state change in any child bg
    } else if (pid == -1) {
        printf("Error: call to waitpid() failed\n");
        return false;
    } else {
        // Find the LL node representing bg process which terminated
        bg_pro_t* process = find_node_by_pid(bg_proc_list, pid);
        print_node(process);
        list_remove(bg_proc_list, process);
        return true;
    }
}

int main() {

    /* Prompt information */
    char* user_login = update_login();
    char host_name[MAX_HOSTNAME_SIZE] = {0};
    char current_directory[MAX_INPUT_SIZE] = {0};

    /* Input buffer, array of char pointers */
    char* user_input[MAX_CMD_NUMBER] = {0};

    /* Prompt to be displayed by readline() */
    char prompt[MAX_INPUT_SIZE];
    char prompt_format[MAX_INPUT_SIZE] = "%s@%s: %s > ";

    /* Initialize Prompt */
    update_hostname(host_name);
    update_cwd(current_directory);

    bool ongoing = true;
    int num_read = 0;

    while (ongoing) {
        /* Update prompt information */
        update_cwd(current_directory);
        sprintf(prompt, prompt_format, user_login, host_name, current_directory);

        /* Parse and execute user input */
        num_read = parse_user_input(user_input, prompt);
        if (num_read) {
            ongoing = execute_user_cmd(user_input, num_read);
            clear_user_input(user_input, num_read);
        }

        /* Check status of background processes */
        check_bg_proc_status();
    }

    /* Teardown and exit gracefully */
    if (bg_proc_list != NULL) {
        destroy_list(bg_proc_list);
    }

    return 0;
}
