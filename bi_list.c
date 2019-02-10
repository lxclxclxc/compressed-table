#include "bi_list.h"
#include "debug.h"
#include<stdlib.h>
//this below is for the request's missing subrequests in mapping table buffer and data buffer
//node
struct _bi_node* create_and_init_bi_node(int lpn, int state)
{
    struct _bi_node* new_node = (struct _bi_node*)malloc(sizeof(struct _bi_node));
    new_node->prevNode = NULL;
    new_node->nextNode = NULL;
    new_node->lpn_and_state[0] = lpn;
    new_node->lpn_and_state[1] = state;
    return new_node;
}


void del_bi_node(struct _bi_node* node)
{
    if((node->nextNode == NULL) && (node->prevNode == NULL)){

        free(node);
        node = NULL;

    }else{
        //tail deletion
        if((node->nextNode == NULL) && (node->prevNode != NULL)){
            node->prevNode->nextNode = NULL;
        }
        //head deletion

        if((node->nextNode != NULL) && (node->prevNode == NULL)){
            node->nextNode->prevNode = NULL;
            //medium deletion
        }
        free(node);
        node = NULL;
    }
}

//create_list_and_initial_list
struct _bi_list* create_and_init_bi_list(void)
{
    struct _bi_list* new_list = (struct _bi_list*)malloc(sizeof(struct _bi_list));
    new_list->node_head = NULL;
    new_list->node_tail = NULL;
    new_list->len = 0;

    return new_list;
}





// insert element in head part
void lpush(struct _bi_list* list, int lpn, int state)
{
    struct _bi_node* new_node = create_and_init_bi_node(lpn, state);

    new_node->nextNode = list->node_head;

    if(list->node_head != NULL){
        list->node_head->prevNode = new_node;

    }
    list->node_head = new_node;
    if(list->len == 0){
        list->node_tail = new_node;
    }
    list->len++;
}

//pop node 

//delete list
void del_bi_list(struct _bi_list* list)
{
    int i = 0;
    struct _bi_node* temp, *temp1;

    if(list->len == 0){
        free(list);
        list = NULL;
    }else{
        temp = list->node_head;
        for(i=0; i<list->len; i++){
            temp1 = temp->nextNode;
            del_bi_node(temp);
            temp = temp1;
            (list->len) --;
        }
        traceError_msg("in bilist.c, deleting list, bi_list len is %d,", list->len);
        //free(list);
        //list = NULL;
    }

}

//this above is for the request's missing subrequests in mapping table buffer and data buffer



