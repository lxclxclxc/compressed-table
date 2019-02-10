#ifndef _JS_LIST_H
#define _JS_LIST_H

#define TYPE unsigned int 

struct nodeJSList
{
	struct nodeJSList* prevNode;
	struct nodeJSList* nextNode;
	TYPE value;
};

struct jsList
{
	int numNode;
	struct nodeJSList* head;
	struct nodeJSList* tail;
};

struct nodeJSList* createNode(TYPE newValue);
void initList(struct jsList* list);
struct nodeJSList* getNode(int index, struct jsList* list);
int insertNode(int index, struct nodeJSList* newNode, struct jsList* list);
void deleteNode(struct nodeJSList* deletedNode, struct jsList* list);
#endif
