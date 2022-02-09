#include "linked_list.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/* Initializers */
bg_pro_t* create_node(pid_t proc_id, char command[1024]) {
    bg_pro_t* node = (bg_pro_t*) malloc(sizeof(bg_pro_t));
    if (node == NULL){
        printf("Error: malloc() cannot allocate space for new background process.\n");
        exit(1);
    }
    node->pid = proc_id;
    strncpy(node->cmd, command, 1024*sizeof(char));
    node->next = NULL;

    return node;
}

bg_list_t* create_list(bg_pro_t* bg_head) {
    if (bg_head == NULL) {
        printf("Error: cannot create new list from NULL head node.\n");
        exit(1);
    }
    bg_list_t* list = (bg_list_t*) malloc(sizeof(bg_list_t));
    if (list == NULL) {
        printf("Error: malloc() cannot allocate space for new linked list.\n");
        exit(1);
    }
    list->head = bg_head;
    return list;
}

void destroy_list(bg_list_t* list) {
    bg_pro_t* temp;
    if (list == NULL) {
        return;
    }
    while (list->head != NULL) {
        temp = list->head;
        list->head = list->head->next;
        free(temp);
    }
    free(list);
}

/* Module functions */
bg_list_t* list_append(bg_list_t* list, bg_pro_t* node) {
    if (list == NULL) {
        printf("Error: cannot append to NULL list.\n");
        exit(1);
    }
    if (list->head->next == NULL) {
        list->head->next = node;
    } else {
        bg_pro_t* temp;
        for (temp = list->head->next; temp->next != NULL; temp = temp->next) {/* Empty loop */}
        temp->next = node;
    }
    return list;
}

bg_pro_t* find_node_by_pid(bg_list_t* list, pid_t proc_id) {
    bg_pro_t* node = list->head;
    while (node->pid != proc_id) {
        node = node->next;
    }
     return node;
}

bg_list_t* list_remove(bg_list_t* list, bg_pro_t* node) {
    bg_pro_t* temp = list->head;
    while(temp->next != NULL) {
        if (node == temp->next) {
            bg_pro_t* to_remove = temp->next;
            temp->next = temp->next->next;
            free(to_remove);
        }
        temp = temp->next;
    }
    return list;
}

int list_length(bg_list_t* list) {
    int list_length = 0;
    if (list->head == NULL) {
        return list_length;
    }
    bg_pro_t* temp = list->head;
    while (temp != NULL) {
        temp = temp->next;
        list_length++;
    }
    return list_length;
}

void list_print(bg_list_t* list) {
    bg_pro_t* temp = list->head;
    int index = 0;
    while (temp != NULL) {
        printf("List index: %d\n", index);
        printf("Proc ID: %d\n", temp->pid);
        printf("Command: %s\n\n", temp->cmd);
        temp = temp->next;
        index++;
    }
}

