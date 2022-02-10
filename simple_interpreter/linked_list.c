#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_list.h"

/* Initializers */
bg_pro_t* create_node(pid_t proc_id, char* command[MAX_CMD_NUMBER], int num_args) {
    bg_pro_t* node = (bg_pro_t*) malloc(sizeof(bg_pro_t));
    if (node == NULL){
        printf("Error: malloc() cannot allocate space for new background process.\n");
        exit(1);
    }
    node->pid = proc_id;
    for (int i = 0; i < num_args; i++){
        strncpy(node->argv[i], command[i], MAX_INPUT_SIZE);
    }
    node->next = NULL;
    node->num_args = num_args;
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
    list->length = 1;
    return list;
}

/* Memory freeing at end of life */
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

/* Module API functions */
bg_list_t* list_append(bg_list_t* list, bg_pro_t* node) {
    if (list == NULL) {
        printf("Error: cannot append to NULL list.\n");
        exit(1);
    }
    if (list->length == 0) {
        list->head = node;
        list->length = 1;
        return list;
    }
    if (list->head->next == NULL) {
        list->head->next = node;
    } else {
        bg_pro_t* temp;
        for (temp = list->head->next; temp->next != NULL; temp = temp->next) {/* Empty loop */}
        temp->next = node;
    }
    list->length++;
    return list;
}

bg_pro_t* find_node_by_pid(bg_list_t* list, pid_t proc_id) {
    if (list->head == NULL) {
        return NULL;
    }
    bg_pro_t* node = list->head;
    while (node != NULL) {
        if (node->pid == proc_id) {
            break;
        }
        node = node->next;
    }
    return node;
}

bg_list_t* list_remove(bg_list_t* list, bg_pro_t* node) {
    if (node == NULL) {
        printf("Error: cannot remove null node.\n");
        exit(1);
    }
    if (list->head == NULL) {
        list->length = 0;
        return list;
    }
    if (node == list->head) {
        bg_pro_t* temp = list->head;
        list->head = NULL;
        free(temp);
        list->length = 0;
        return list;
    }
    bg_pro_t* temp = list->head;
    while(temp->next != NULL) {
        if (node == temp->next) {
            if (temp->next->next == NULL) {
                // Removing last item of list
                bg_pro_t* to_remove = temp->next;
                temp->next = NULL;
                free(to_remove);
                list->length--;
                return list;
            } else {
                // Somewhere in the middle of the list
                bg_pro_t *to_remove = temp->next;
                temp->next = temp->next->next;
                free(to_remove);
            }
        }
        temp = temp->next;
    }
    list->length--;
    return list;
}

/* Utility functions */
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

/* For debugging purposes */
void list_print(bg_list_t* list) {
    if (list->head == NULL) {
        printf("Cannot print NULL list.\n");
        return;
    }
    bg_pro_t* temp = list->head;
    int index = 0;
    while (temp != NULL) {
        printf("List index: %d  --  ", index);
        printf("Proc ID: %d  --  ", temp->pid);
        for (int i = 0; i < temp->num_args; i++) {
            if (temp->argv[i] != NULL) {
                printf(" %s", temp->argv[i]);
            }
        }
        printf("\n");
        temp = temp->next;
        index++;
    }
}

/* Implements 'bglist' command */
void process_print(bg_list_t* list) {
    if (list == NULL) {
        printf("Total Background jobs: 0\n");
        return;
    }
    bg_pro_t* temp = list->head;
    int index = 0;
    while (temp != NULL) {
        printf("%d: %s ", temp->pid, temp->argv[0]);
        for (int i = 1; i < temp->num_args; i++) {
            if (temp->argv[i] != NULL) {
                printf("%s ", temp->argv[i]);
            }
        }
        printf("\n");
        temp = temp->next;
        index++;
    }
    printf("Total Background jobs: %d\n", index);
}

void print_node(bg_pro_t* node) {
    printf("%d: %s ", node->pid, node->argv[0]);
    for (int i = 1; i < node->num_args; i++) {
        if (node->argv[i] != NULL) {
            printf("%s ", node->argv[i]);
        }
    }
    printf("has terminated\n");
}
