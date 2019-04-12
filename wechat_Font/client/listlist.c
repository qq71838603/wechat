

/*
test a double-linked circular list
*/
#include"header.h"
struct listlist
{
	void *listhead;
	char listname[20];
	struct listlist *next;
	struct listlist *preview;
};
typedef struct listlist listlist;

listlist *listlist_init()
{
	listlist *head=malloc(sizeof(listlist));
	head->next=head;
	head->preview=head;
	return head;
}

int listhead_insert(void *listhead,char *listname,listlist *mylist)
{
	listlist *p = mylist;
	listlist *tail = mylist->preview;
	listlist *newnode = malloc(sizeof(listlist));
	newnode->listhead=listhead;
	strcpy(newnode->listname,listname);
	newnode->preview=tail;
	newnode->next=p;
	p->preview=newnode;
	tail->next=newnode;
	return 0;
}

int listhead_quiery(listlist *mylist)
{
	listlist *p = mylist;
	do{
		p=p->next;
		printf("%s\n",p->listname);
	}while(p->next!=mylist);
}

int listhead_del(listlist *delnode,listlist* mylist)
{
	listlist *p=mylist;
	delnode->next->preview=delnode->preview;
	delnode->preview->next=delnode->next;
	delnode->next=NULL;
	delnode->preview=NULL;
	free(delnode);
}

int listhead_backquiery(listlist *mylist)
{
	listlist *p = mylist;
	do{
		p=p->preview;
		printf("%s\n",p->listname);
	}while(p->preview!=mylist);
}
/* 
void main(int argc,char **argv)
{
	listlist *infohead=listlist_init();
	listhead_insert(NULL,"test",infohead);
	listhead_quiery(infohead);
	free(infohead);
} */