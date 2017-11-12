%{
	#include<stdio.h>
	#include<stdlib.h>
	#include<string.h>
	#include"syntax.tab.h"
	#include"node.h"
	int flag=0;
	extern int ErrorNum;
%}
%option yylineno

SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS "+"
MINUS "-"
STAR "*"
DIV "/"
AND "&&"
OR "||"
DOT "."
NOT "!"
TYPE int|float
LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
SPACE [ \n\t\r]+
DEC [1-9][0-9]*|0
OCT 0[0-7]*
WRONG_OCT 0[0-9a-wyzA-WYZ][0-9A-Za-z]*
HEX 0[xX][0-9a-fA-F]*
WRONG_HEX 0[xX][0-9a-zA-Z]*
INT {DEC}|{OCT}|{HEX}
SCI_NOT [+|-]?[0-9]*[.][0-9]*[E|e][+|-]?[0-9]*
FLOAT [0-9]*[.][0-9]+f?|{SCI_NOT}
ID [A-Za-z_][A-Za-z_0-9]*
OTHER .
SINGLE_LINE_COMMENT "//"
MULTI_LINE_COMMENT "/*"
COMMENT_ERROR "*/"

%%

{SEMI}	{
    yylval.node=CreateNode("SEMI",yytext);
    return(SEMI);
}
{COMMA} {
    yylval.node=CreateNode("COMMA",yytext);
    return(COMMA);
}
{ASSIGNOP} {
    yylval.node=CreateNode("ASSIGNOP",yytext);
    return ASSIGNOP;
}


{RELOP}     {
    yylval.node=CreateNode("RELOP",yytext);
    return RELOP;
}

{PLUS}      {
    yylval.node=CreateNode("PLUS",yytext);
    return PLUS;
}
{MINUS}     {
    yylval.node=CreateNode("MINUS",yytext);
    return MINUS;
}
{STAR}      {
    yylval.node=CreateNode("STAR",yytext);
    return STAR;
}
{DIV}       {
    yylval.node=CreateNode("DIV",yytext);
    return DIV;
}
{AND}       {
    yylval.node=CreateNode("AND",yytext);
    return AND;
}
{OR}        {
    yylval.node=CreateNode("OR",yytext);
    return OR;
}
{DOT}       {
    yylval.node=CreateNode("DOT",yytext);
    return DOT;
}
{NOT}       {
    yylval.node=CreateNode("NOT",yytext);
    return NOT;
}
{TYPE}      {
    yylval.node=CreateNode("TYPE",yytext);
    return TYPE;
}
{LP}        {
    yylval.node=CreateNode("LP",yytext);
    return LP;
}
{RP}        {
    yylval.node=CreateNode("RP",yytext);
    return RP;
}
{LB}        {
    yylval.node=CreateNode("LB",yytext);
    return LB;
}
{RB}        {
    yylval.node=CreateNode("RB",yytext);
    return RB;
}
{LC}        {
    
    yylval.node=CreateNode("LC",yytext);
    return LC;
}
{RC}        {
    yylval.node=CreateNode("RC",yytext);
    return RC;
}
{STRUCT}    {
    yylval.node=CreateNode("STRUCT",yytext);
    return STRUCT;
}
{RETURN}    {
    yylval.node=CreateNode("RETURN",yytext);
    return RETURN;
}
{IF}        {
    yylval.node=CreateNode("IF",yytext);
    return IF;
}
{ELSE}      {
    yylval.node=CreateNode("ELSE",yytext);
    return ELSE;
}
{WHILE}     {
    yylval.node=CreateNode("WHILE",yytext);
    return WHILE;
}
{SPACE}     {}


{OCT}       {
    //char *end = NULL;
    int l = (int)strtol(yytext, NULL, 8);
    snprintf(yytext, 10,"%d",l);
    yylval.node=CreateNode("INT",yytext);
    return INT;
}
{WRONG_OCT} {
    ErrorNum++;
    printf("Error type A at Line %d: Illegal octal number '%s'\n", yylineno, yytext);
    return INT;
}
{HEX}       {
    //char *end = NULL;
    int l = (int)strtol(yytext, NULL, 16);
    snprintf(yytext, 10,"%d",l);
    yylval.node=CreateNode("INT",yytext);
    return INT;
}
{WRONG_HEX} {
    ErrorNum++;
    printf("Error type A at Line %d: Illegal hexadecimal number '%s'\n", yylineno, yytext);
    return INT;
}
{INT}       {
    yylval.node=CreateNode("INT",yytext);
    return INT;
}
{SCI_NOT}	{
	printf("%s\n",yytext);
	int start=0,pos=1,exp=0;
	float value=0,base=0;
	char str[20];
	for(;yytext[start]!='E'&&yytext[start]!='e';++start);
	printf("start=%d\n",start);
	strncpy(str,yytext,start);
	str[start]='\0';
	base=atof(str);
	value=base;
	printf("base=%f\n",base);
	start++;
	if(yytext[start]=='-')
		pos=0;
	int i=start;
	for(;i<strlen(yytext);++i)
		yytext[i-start]=yytext[i];
	yytext[i-start]='\0';
	exp=atoi(yytext);
	printf("exp=%d\n",exp);
	if(pos==1)
	{
		for(;exp>0;--exp)
			value=value*10.0;
	}
	else
	{
		for(;exp<0;++exp)
			value=value/10.0;
	}
	printf("value=%f\n",value);
	snprintf(yytext,10,"%f",value);
    yylval.node=CreateNode("FLOAT",yytext);
	return FLOAT;
}
{FLOAT}     {
    yylval.node=CreateNode("FLOAT",yytext);
    return FLOAT;
}

{ID}        {
    
    yylval.node=CreateNode("ID",yytext);
    return ID;
}
{OTHER} {
    ErrorNum++;
    printf("Error type A at line %d: Mysterious character \"%s\".\n",yylineno,yytext);
}
{SINGLE_LINE_COMMENT}	{
	char c=input();
	while(c!='\n')
		c=input();
}
{MULTI_LINE_COMMENT}	{
	flag++;
	char c=input();
	char h='0';
	while(c!='/'||h!='*')
	{
		if(h=='/'&&c=='*')
		{
			flag+=1;
			c=input();
			//printf("hello\n");
		}
		h=c;
		c=input();
		/*if(c=='/'&&h=='*'&&flag==1)
		{
			ErrorNum++;
			printf("Error type B at line %d:find matched comment in other comment.\n",yylineno);
		}*/
		//printf("h:%c,c=%c.\n",h,c);
	}
	flag--;
	//printf("flag:%d.\n",flag);
}
{COMMENT_ERROR}		{
	ErrorNum++;
	if(flag>0)
		printf("Error type B at line %d:find matched comment in other comment.\n",yylineno);
	else
		printf("Error type B at line %d:find an unmatched %s.\n",yylineno,yytext);
	flag--;
	if(flag<0)
		flag=0;
}
%%

int yywrap() {
    return 1;
}
