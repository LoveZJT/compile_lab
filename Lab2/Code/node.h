/*************************************************************************
	> File Name: ../../../compile_lab/node.h
	> Author: LoveZJT
	> Mail: 151220055@smail.nju.edu.cn
	> Created Time: 2017年11月07日 星期二 21时45分20秒
 ************************************************************************/

#pragma once
#include<string.h>
#include<stdlib.h>
extern int yylineno;

typedef struct Tree
{
	char type[32];
	char content[32];
	int line;
	struct Tree *parent;
	struct Tree *children[5];
	int childnum;
}Node;

Node* CreateNode(char *type,char *content);
void AddChild(int num,Node *parent,...);
void PrintTree(Node *root,int blank);
