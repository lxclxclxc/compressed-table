#ifndef  _BI_LIST_H
#define _BI_LIST_H
//this below is for the request's missing subrequests in mapping table buffer and data buffer
//node
#include <stdio.h>
#include<stdlib.h>

typedef struct _bi_node {
struct _bi_node* prevNode;
struct _bi_node* nextNode;
int lpn_and_state[2];

}bi_node;
//initialize bi_node

struct _bi_node* create_and_init_bi_node(int lpn, int state);

void del_bi_node(struct _bi_node* node);

//list 
typedef struct _bi_list{
struct _bi_node* node_head;
struct _bi_node* node_tail;
int len;

}bi_list;
//create_list_and_initial_list
struct _bi_list* create_and_init_bi_list(void);



// insert element in head part
void lpush(struct _bi_list* list, int lpn, int state);


//pop node 

//delete list
void del_bi_list(struct _bi_list* list);


//this above is for the request's missing subrequests in mapping table buffer and data buffer

#endif //end of _BI_LIST_H
