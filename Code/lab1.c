/*************************************************************************
	> File Name: lab1.c
	> Author: LoveZJT
	> Mail: 151220055@smail.nju.edu.cn
	> Created Time: 2017年10月30日 星期一 20时36分46秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include"node.h"

extern void yyrestart(FILE*);
extern int yyparse();
//extern int yylineno;

Node *root=NULL;
int ErrorNum=0;

void MyWrong(char *message){
        printf("Error type B at line %d: %s\n", yylineno, message);
}

int main(int argc,char **argv)
{
	if(argc<2)
	{
		printf("Program missing parameters.\n");
		return 0;
	}
	FILE *fp=fopen(argv[1],"r");
	if(!fp)
	{
		printf("%s: No such file or directory",argv[1]);
		return 0;
	}
	yylineno=1;
	yyrestart(fp);
	yyparse();

	if(ErrorNum==0)
		PrintTree(root,0);
	return 0;
}
