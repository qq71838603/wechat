

/*
test a double-linked circular list
*/
#include"header.h"
struct infolist
{
	attr *attribute;
	struct infolist *next;
	struct infolist *preview;
};
typedef struct infolist stu;
stu *all_user_list;

stu *init_list()
{
	stu *head=malloc(sizeof(stu));
	head->next=head;
	head->preview=head;
	head->attribute=NULL;
	return head;
}

int reset_list(stu *head)
{
	stu *p=head;
	if(head->next!=head)
	{
		p=head->next;
		head->next=head;
		head->preview=head;
		free(p);
	}
}

int tail_insert(attr *attribute,stu *mylist)
{
	stu *p=mylist;
	stu *tail = mylist->preview;

	while(p->next!=mylist)
	{
		p=p->next;
		if(strcmp((char *)attribute->nicname,p->attribute->nicname)==0)
		{
			printf("repeated!\n");
			return -1;
		}
	}
	stu *newnode = malloc(sizeof(stu));
	newnode->preview=tail;
	newnode->next=mylist;
	newnode->attribute=attribute;
	mylist->preview=newnode;
	tail->next=newnode;
	return 0;
}

int quiery(stu *mylist)
{
	stu *p = mylist;
	int i=0;
	while(1)
	{
		i++;
		p=p->next;
		printf("user%d: %s\n",i,p->attribute->nicname);
		if(p->next!=mylist){
			break;
		}
	}
}

attr *get_user_bycount(int count,stu *mylist)
{
	if(mylist==NULL)
		mylist=all_user_list;
	stu *p = mylist;
	int i=0;
	while(1)
	{
		i++;
		p=p->next;
		if(i==count){
			return p->attribute;
		}
		else if(p->next==mylist)
			return NULL;
	}
}

char *get_user_byname(int num,int maxnum,stu *mylist)
{
	stu *p = mylist;
	int i=0;
	while(1)
	{
		i++;
		p=p->next;
		printf("user%d: %s\n",i,p->attribute->nicname);
		if(i==num)
			return p->attribute->nicname;
		if(p->next!=mylist){
			break;
		}
	}
}

int list_del(stu *delnode,stu* mylist)
{
	stu *p=mylist;
	delnode->next->preview=delnode->preview;
	delnode->preview->next=delnode->next;
	delnode->next=NULL;
	delnode->preview=NULL;
	free(delnode->attribute);
	free(delnode);
}


int backquiery(stu *mylist)
{
	stu *p = mylist;
	do{
		p=p->preview;
		printf("%s\n",p->attribute->nicname);
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
