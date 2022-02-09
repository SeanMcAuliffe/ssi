#ifndef OPERATING_SYSTEMS_LINKED_LIST_H
#define OPERATING_SYSTEMS_LINKED_LIST_H

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

/* Node Type */
typedef struct bg_pro {
    struct bg_pro* next;
    pid_t pid;
    char cmd[1024];
} bg_pro_t;

/* List type */
typedef struct bg_list {
    bg_pro_t* head;
} bg_list_t;

/* Initializers */
bg_pro_t* create_node(pid_t pid, char command[1024]);
bg_list_t* create_list(bg_pro_t* head);
void destroy_list(bg_list_t* list);

/* Insert new node */
bg_list_t* list_append(bg_list_t* list, bg_pro_t* node);

/* Remove node */
bg_list_t* list_remove(bg_list_t* list, bg_pro_t* node);

/* Helpers */
int list_length(bg_list_t* list);
bg_pro_t* find_node_by_pid(bg_list_t* list, pid_t proc_id);
void list_print(bg_list_t* list);

#endif //OPERATING_SYSTEMS_LINKED_LIST_H
