

/*
test a double-linked circular list
*/
#include"header.h"
struct msglist
{
	char *msg;
	struct msglist *next;
	struct msglist *preview;
};
typedef struct msglist msglist;
msglist *msg_head;
msglist *msglist_init()
{
	msglist *head=malloc(sizeof(msglist));
	head->next=head;
	head->preview=head;
	head->msg=NULL;
	return head;
}

int msg_insert(char * *msg,msglist *mylist)
{
	msglist *p=mylist;
	msglist *tail = mylist->preview;
	msglist *newnode = malloc(sizeof(msglist));
	newnode->preview=tail;
	newnode->next=mylist;
	
	strcpy(newnode->msg,msg);
	mylist->preview=newnode;
	tail->next=newnode;
	return 0;
}

int msg_quiery(msglist *mylist)
{
	msglist *p = mylist;
	int i=0;
	while(1)
	{
		i++;
		p=p->next;
		printf("user%d: %s\n",i,p->msg);
		if(p->next!=mylist){
			break;
		}
	}
}

int msglist_del(msglist *delnode,msglist* mylist)
{
	msglist *p=mylist;
	delnode->next->preview=delnode->preview;
	delnode->preview->next=delnode->next;
	delnode->next=NULL;
	delnode->preview=NULL;
	free(delnode->msg);
	free(delnode);
}


int msg_backquiery(msglist *mylist)
{
	msglist *p = mylist;
	do{
		p=p->preview;
		printf("%s\n",p->msg);
	}while(p->preview!=mylist);
}
/* void main(int argc,char **argv)
{
	infohead=init_list();
	tail_insert("hello",1,infohead);
	quiery(infohead);
	//backquiery(mylist);
	free(infohead);
} */
