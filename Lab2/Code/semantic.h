/*************************************************************************
	> File Name: semantic.h
	> Author: LoveZJT
	> Mail: 151220055@smail.nju.edu.cn
	> Created Time: 2017年11月30日 星期四 15时07分42秒
 ************************************************************************/

#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"node.h"

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;

struct Type_
{
	enum { BASIC, ARRAY, STRUCTURE, FUNCTION } kind;
	union
	{
		int basic;
		struct { Type elem; int size; } array;
		FieldList structure;
		struct { FieldList param; Type funcType; int num; } function;
	}u;
}Type_;

typedef struct FieldList_
{
	char *name;
	Type type;
	FieldList tail;
	int offset;
}FieldList_;

void traversetree(Node*);
FieldList vardec(Node* ,Type );
Type specifier(Node*);
void extdeflist(Node*);
void compst(Node*,Type);
void deflist(Node*);
void stmt(Node*,Type);
Type exp(Node*);

unsigned int hash_pjw(char *);
int insert(FieldList );
int equal(Type,Type);
FieldList find(char* , int );

