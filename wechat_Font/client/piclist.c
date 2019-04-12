#include"header.h"
struct piclist
{
	char picname[20];
	int *picbuf;
	struct piclist *next;
	struct piclist *preview;
};
typedef struct piclist piclist;

piclist *piclist_head;

piclist *init_piclist(char *picname,int *picbuf)
{
	piclist *head=malloc(sizeof(piclist));
	if(picname!=NULL)
		strcpy(head->picname,picname);
	head->picbuf=picbuf;
	head->next=head;
	head->preview=head;
	return head;
}
int piclist_insert(char *name,int *picbuf,piclist *mylist)
{
	piclist *p=mylist;
	piclist *tail = mylist->preview;
	while(p->next!=mylist)
	{
		p=p->next;
		if(strcmp(name,p->picname)==0)
		{
			printf("repeated!\n");
			return -1;
		}
	}
	piclist *newnode = malloc(sizeof(piclist));
	strcpy(newnode->picname,name);
	newnode->picbuf=picbuf;
	newnode->preview=tail;
	newnode->next=mylist;
	mylist->preview=newnode;
	tail->next=newnode;
	return 0;
}
int piclist_quiery(piclist *mylist)
{
	piclist *p = mylist;
	while(p->next!=mylist)
	{
		p=p->next;
		printf("%s\n",p->picname);
	}
}

int piclist_del(piclist *delnode,piclist* mylist)
{
	piclist *p=mylist;
	delnode->next->preview=delnode->preview;
	delnode->preview->next=delnode->next;
	delnode->next=NULL;
	delnode->preview=NULL;
	free(delnode);
}

int piclist_del_byname(char *picname,piclist* mylist)
{
	piclist *p=mylist;
	while(p->next!=mylist)
	{
		p=p->next;
		if(strcmp(p->picname,picname)==0)
		{
			piclist_del(p,mylist);
			return 0;
		}
	}
}

int *piclist_find_byname(char *picname,piclist* mylist)
{
	piclist *p=mylist;
	do{
		if(strcmp(p->picname,picname)==0)
		{
			return mylist->picbuf;
		}
		p=p->next;
	}while(p!=mylist);
	if(strcmp(p->picname,picname)!=0)
		return NULL;
}


int piclist_backquiery(piclist *mylist)
{
	piclist *p = mylist;
	do{
		p=p->preview;
		printf("%s\n",p->picname);
	}while(p->preview!=mylist);
}
//test
/* void main()
{
	piclist *head=init_list();
	piclist_insert("test1",NULL,head);
	piclist_insert("test2",NULL,head);
	piclist_insert("test3",NULL,head);
	piclist_quiery(head);
	piclist_del_byname("test1",head);
	piclist_backquiery(head);
} */