%{
    #include <stdio.h>
    #include "node.h"
    #include "lex.yy.c"
    extern int yylineno;
    void MyWrong(char *msg);
    extern Node* root;
    extern int ErrorNum;
    int yyerror(char const *msg);
%}

%union {
    struct Tree* node;
}

//terminal token
%token <node> INT FLOAT ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV 
%token <node> AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE

//association
%right ASSIGNOP  
%left OR 
%left AND 
%left RELOP
%left PLUS MINUS 
%left STAR DIV
%right NOT
%left DOT LB RB LP RP

//noassociation
%nonassoc LOWER_THAN_ELSE 
%nonassoc ELSE

//non-terminal type
%type <node> Program ExtDefList ExtDef ExtDecList Specifier
%type <node> StructSpecifier OptTag Tag VarDec FunDec VarList
%type <node> ParamDec CompSt StmtList Stmt DefList Def DecList
%type <node> Dec Exp Args




%%

//High-level Definitions

Program     : ExtDefList        {$$=CreateNode("Program","");AddChild(1, $$, $1);root=$$;}
            ;

ExtDefList  : ExtDef ExtDefList     {$$=CreateNode("ExtDefList","");AddChild(2, $$, $1, $2);}
            |  /* empty*/           {$$=NULL;}
            ;
                
ExtDef      : Specifier ExtDecList SEMI     {$$=CreateNode("ExtDef","");AddChild(3, $$, $1, $2, $3);}
            | Specifier SEMI                {$$=CreateNode("ExtDef","");AddChild(2, $$, $1, $2);}    
            | Specifier FunDec CompSt       {$$=CreateNode("ExtDef","");AddChild(3, $$, $1, $2, $3);}
            | error SEMI                    {ErrorNum++;MyWrong("Syntax error.");}
            ;
ExtDecList  : VarDec                        {$$=CreateNode("ExtDecList","");AddChild(1, $$, $1);} 
            | VarDec COMMA ExtDecList       {$$=CreateNode("ExtDecList","");AddChild(3, $$, $1, $2, $3);} 
            ;

            

//Specifiers

Specifier   : TYPE                  {$$=CreateNode("Specifier","");AddChild(1, $$, $1);}
            | StructSpecifier       {$$=CreateNode("Specifier","");AddChild(1, $$, $1);}
            ;
            
StructSpecifier : STRUCT OptTag LC DefList RC   {$$=CreateNode("StructSpecifier","");AddChild(5, $$, $1, $2, $3, $4, $5);}
                | STRUCT Tag                    {$$=CreateNode("StructSpecifier","");AddChild(2, $$, $1, $2);}
                ;
            
OptTag  : ID                {$$=CreateNode("OptTag","");AddChild(1, $$, $1);}
        | /* empty*/        {$$=NULL;}
        ;
        
Tag     : ID                {$$=CreateNode("Tag","");AddChild(1, $$, $1);}
        ;


//4.Declarators

VarDec      : ID                            {$$=CreateNode("VarDec","");AddChild(1, $$, $1);}
            | VarDec LB INT RB              {$$=CreateNode("VarDec","");AddChild(4, $$, $1, $2, $3, $4);}
            | VarDec LB error RB            {ErrorNum++; MyWrong("Missing ].");}
            ;
            
FunDec      : ID LP VarList RP              {$$=CreateNode("FunDec","");AddChild(4, $$, $1, $2, $3, $4);}
            | ID LP RP                      {$$=CreateNode("FunDec","");AddChild(3, $$, $1, $2, $3);}
            | error RP                      {ErrorNum++;MyWrong("Syntax error.");}
            ;
            
VarList     : ParamDec COMMA VarList        {$$=CreateNode("VarList","");AddChild(3, $$, $1, $2, $3);}
            | ParamDec                      {$$=CreateNode("VarList","");AddChild(1, $$, $1);}
            ;
            
ParamDec    : Specifier VarDec              {$$=CreateNode("ParamDec","");AddChild(2, $$, $1, $2);}
            | error COMMA                   {ErrorNum++;MyWrong("Missing ;.");}
            | error RP                      {ErrorNum++;MyWrong("Missing ).");}
            ;

            
//Statements

CompSt      : LC DefList StmtList RC        {$$=CreateNode("CompSt","");AddChild(4, $$, $1, $2, $3, $4);}
            | LC error RC                   {ErrorNum++;MyWrong("Syntax error.");}
            ;
            
StmtList    : Stmt StmtList                 {$$=CreateNode("StmtList","");AddChild(2, $$, $1, $2);}
            | /*empty*/                     {$$=NULL;}
            ;
            
Stmt    : Exp SEMI                                      {$$=CreateNode("Stmt","");AddChild(2, $$, $1, $2);}
        | CompSt                                        {$$=CreateNode("Stmt","");AddChild(1, $$, $1);}
        | RETURN Exp SEMI                               {$$=CreateNode("Stmt","");AddChild(3, $$, $1, $2, $3);}
        | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE       {$$=CreateNode("Stmt","");AddChild(5, $$, $1, $2, $3, $4, $5);}
        | IF LP Exp RP Stmt ELSE Stmt                   {$$=CreateNode("Stmt","");AddChild(7, $$, $1, $2, $3, $4, $5, $6, $7);}
        | IF LP Exp RP error ELSE Stmt                  {ErrorNum++;MyWrong("Missing ;.");}
        | WHILE LP Exp RP Stmt                          {$$=CreateNode("Stmt","");AddChild(5, $$, $1, $2, $3, $4, $5);}
        ;


//Local Definitions

DefList : Def DefList               {$$=CreateNode("DefList","");AddChild(2, $$, $1, $2);}
        | /*empty*/                 {$$=NULL;}
        ;
        
Def     : Specifier DecList SEMI    {$$=CreateNode("Def","");AddChild(3, $$, $1, $2, $3);}
        //| error SEMI              {ErrorNum++;MyWrong("Syntax error.");}
        ;

DecList : Dec                       {$$=CreateNode("DecList","");AddChild(1, $$, $1);}
        | Dec COMMA DecList         {$$=CreateNode("DecList","");AddChild(3, $$, $1, $2, $3);}
        ;
        
Dec     : VarDec                    {$$=CreateNode("Dec","");AddChild(1, $$, $1);}
        | VarDec ASSIGNOP Exp       {$$=CreateNode("Dec","");AddChild(3, $$, $1, $2, $3);}
        ;
            
            
//Expressions
Exp     : Exp ASSIGNOP Exp      {$$=CreateNode("Exp","");AddChild(3, $$, $1, $2, $3);}
        | Exp AND Exp           {$$=CreateNode("Exp","");AddChild(3, $$, $1, $2, $3);}
        | Exp OR Exp            {$$=CreateNode("Exp","");AddChild(3, $$, $1, $2, $3);}
        | Exp RELOP Exp         {$$=CreateNode("Exp","");AddChild(3, $$, $1, $2, $3);}
        | Exp PLUS Exp          {$$=CreateNode("Exp","");AddChild(3, $$, $1, $2, $3);}
        | Exp MINUS Exp         {$$=CreateNode("Exp","");AddChild(3, $$, $1, $2, $3);}
        | Exp STAR Exp          {$$=CreateNode("Exp","");AddChild(3, $$, $1, $2, $3);}
        | Exp DIV Exp           {$$=CreateNode("Exp","");AddChild(3, $$, $1, $2, $3);}
        | LP Exp RP             {$$=CreateNode("Exp","");AddChild(3, $$, $1, $2, $3);}
        | MINUS Exp             {$$=CreateNode("Exp","");AddChild(2, $$, $1, $2);}
        | NOT Exp               {$$=CreateNode("Exp","");AddChild(2, $$, $1, $2);}
        | ID LP Args RP         {$$=CreateNode("Exp","");AddChild(4, $$, $1, $2, $3, $4);}
        | ID LP RP              {$$=CreateNode("Exp","");AddChild(3, $$, $1, $2, $3);}
        | Exp LB Exp RB         {$$=CreateNode("Exp","");AddChild(4, $$, $1, $2, $3, $4);}
        | Exp LB error RB       {ErrorNum++;MyWrong("Missing ].");}
        | Exp DOT ID            {$$=CreateNode("Exp","");AddChild(3, $$, $1, $2, $3);}
        | ID                    {$$=CreateNode("Exp","");AddChild(1, $$, $1);}
        | INT                   {$$=CreateNode("Exp","");AddChild(1, $$, $1);}
        | FLOAT                 {$$=CreateNode("Exp","");AddChild(1, $$, $1);}
        ;
    
Args        : Exp COMMA Args        {$$=CreateNode("Exp","");AddChild(3, $$, $1, $2, $3);}
            | Exp                   {$$=CreateNode("Exp","");AddChild(1, $$, $1);}
            ;

%%

int yyerror(char const *msg){
    //printf("Error type B at line %d: %s\n", yylineno, msg);
}
