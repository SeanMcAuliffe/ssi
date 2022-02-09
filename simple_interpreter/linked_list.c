#include "linked_list.h"

/* Initializers */
bg_pro_t* create_node(pid_t proc_id, char command[1024]) {
    bg_pro_t* node = (bg_pro_t*) malloc(sizeof(bg_pro_t));
    if (node == NULL){
        printf("Error: malloc() cannot allocate space for new background process.\n");
        exit(1);
    }
    node->pid = proc_id;
    node->cmd = command;
    node->next = NULL;

    return node;
}

bg_list_t* creat_list(bg_pro_t* bg_head) {
    if (head == NULL) {
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
        for (temp = list->head->next; temp != NULL; temp = temp->next) {/* Empty Loop */}
        temp->next = node;
    }
    return list;
}

bg_pro_t* find_node_by_pid(bg_list_t* list, pid_t proc_id) {
    bg_pro_t node = list->head;
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
    bg_pro_t* temp = list->head;
    while (temp->next != NULL) {
        temp = temp->next;
        length++;
    }
    return length;
}


