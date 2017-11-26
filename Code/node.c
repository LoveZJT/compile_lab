/*************************************************************************
	> File Name: node.c
	> Author: LoveZJT
	> Mail: 151220055@smail.nju.edu.cn
	> Created Time: 2017年11月07日 星期二 22时00分03秒
 ************************************************************************/

#include<stdio.h>
#include<stdarg.h>
#include"node.h"

Node* CreateNode(char *type,char *content)
{
	Node *p=(Node*)malloc(sizeof(Node));
	strcpy(p->type,type);
	strcpy(p->content,content);
	p->line=yylineno;
	p->parent=NULL;
	int i=0;
	for(i=0;i<5;++i)
		p->children[i]=NULL;
	p->childnum=0;
	return p;
}

void AddChild(int childnum,Node *parent,...)
{
	va_list ap;
	va_start(ap,parent);
	int i=0;
	for(i=0;i<childnum;++i)
		parent->children[i]=va_arg(ap,Node*);
	parent->childnum=childnum;
	parent->line=parent->children[0]->line;
	va_end(ap);
}

void PrintTree(Node *root,int blank)
{
	if(root==NULL)
		return ;
	int i=0;
	for(i=0;i<blank;++i)
		printf(" ");
	if(root->childnum!=0)
	{
		printf("%s (%d)\n",root->type,root->line);
		for(i=0;i<root->childnum;++i)
			PrintTree(root->children[i],blank+2);
	}
	else
	{
		if(strcmp(root->type,"INT")==0)
			printf("INT: %d\n",atoi(root->content));
		else if(strcmp(root->type,"FLOAT")==0)
			printf("FLOAT: %f\n",atof(root->content));
		else if(strcmp(root->type,"ID")==0||strcmp(root->type,"TYPE")==0)
			printf("%s: %s\n",root->type,root->content);
		else
			printf("%s\n",root->type);
	}
}
