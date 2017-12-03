/*************************************************************************
	> File Name: semantic.c
	> Author: LoveZJT
	> Mail: 151220055@smail.nju.edu.cn
	> Created Time: 2017年11月30日 星期四 15时06分45秒
 ************************************************************************/

#include"semantic.h"

FieldList hash[65536];

unsigned int hash_pjw(char *name)
{
	unsigned int val=0,i;
	for(;*name;++name)
	{
		val=(val<<2)+*name;
		if(i=val&~0x3fff)
			val=(val^(i>>12))&0x3fff;
	}
	return val%65536;
}

int insert(FieldList f)
{
	if(!f||!(f->name))
		return 0;
	unsigned int key=0;
	f->offset=0;
	if(f->type->kind!=FUNCTION)
		key=hash_pjw(f->name);
	else
		key=hash_pjw(f->name+1);
	while(hash[key]!=NULL)
	{
		key=(key+1)%65536;
		f->offset++;
	}
	hash[key]=f;
	return 1;
}

FieldList find(char *name,int is_function)
{
	if(!name)
		return NULL;
	unsigned int key=0;
	if(is_function==0)
		key=hash_pjw(name);
	else
		key=hash_pjw(1+name);
	FieldList temp=hash[key];
	while(temp)
	{
		if(strcmp(name,temp->name)==0)
		{
			if(temp->type->kind==FUNCTION&&is_function==1)
				return temp;
			if(temp->type->kind!=FUNCTION&&is_function==0)
				return temp;
		}
		key=(key+1)%65536;
		temp=hash[key];
	}
	return NULL;
}

int equal(Type t1,Type t2)
{
	if(!t1||!t2)
		return 0;
	if(t1->kind!=t2->kind)
		return 0;
	if(t1->kind==BASIC)
	{
		if(t1->u.basic==t2->u.basic)
			return 1;
		else
			return 0;
	}
	else if(t1->kind==ARRAY)
	{
		if(equal(t1->u.array.elem,t2->u.array.elem)==1)
			return 1;
		else
			return 0;
	}
	else if(t1->kind==STRUCTURE)
	{
		FieldList f1=t1->u.structure,f2=t2->u.structure;
		while(f1&&f2)
		{
			if(equal(f1->type,f2->type)==0)
				return 0;
			f1=f1->tail;
			f2=f2->tail;
		}
		if(f1==NULL&&f2==NULL)
			return 1;
		return 0;
	}
	else if(t1->kind==FUNCTION)
	{
		if(t1->u.function.num!=t2->u.function.num)
			return 0;
		FieldList p1=t1->u.function.param,p2=t2->u.function.param;
		while(p1&&p2)
		{
			if(equal(p1->type,p2->type)==0)
				return 0;
			p1=p1->tail;
			p2=p2->tail;
		}
		if(p1==NULL&p2==NULL)
			return 1;
		return 0;
	}
	else
		return 0;
}

FieldList vardec(Node *root,Type type)
{
	Node *temp=root;
	int i=0;
	for(i=0;strcmp(temp->children[0]->type,"ID")!=0;++i)
		temp=temp->children[0];
	char *name=temp->children[0]->content;
	FieldList f=(FieldList)malloc(sizeof(FieldList_));
	f->name=name;
	if(strcmp(root->children[0]->type,"ID")==0) //not array
	{
		f->type=type;
		return f;
	}
	if(i==1) // 1 row array
	{
		Type t1=(Type)malloc(sizeof(Type_));
		t1->kind=ARRAY;
		t1->u.array.size=atoi(root->children[2]->content);
		t1->u.array.elem=type;
		f->type=t1;
		return f;
	}
	else if(i==2) // 2 row array
	{
		Type t1=(Type)malloc(sizeof(Type_));
		t1->kind=ARRAY;
		t1->u.array.size=atoi(root->children[2]->content);
		t1->u.array.elem=type;
		Type t2=(Type)malloc(sizeof(Type_));
		t2->kind=ARRAY;
		t2->u.array.size=atoi(root->children[0]->children[2]->content);
		t2->u.array.elem=t1;
		f->type=t2;
		return f;
	}
	else
		printf("only 1 or 2 row array supported\n");
}

Type specifier(Node *root)
{
	Type s=(Type)malloc(sizeof(Type_));
	if(strcmp(root->children[0]->type,"TYPE")==0) // int|float
	{
		s->kind=BASIC;
		if(strcmp(root->children[0]->content,"int")==0)
			s->u.basic=0;
		else
			s->u.basic=1;
		return s;
	}
	else // struct
	{
		s->kind=STRUCTURE;
		if(root->children[0]->childnum==2) // STRUCT tag
		{
			char *name=root->children[0]->children[1]->children[0]->content;
			FieldList f=find(name,0);
			if(!f)
			{
				printf("Error type 17 at line %d: Undefined structure \"%s\".\n",root->line,name);
				s->u.structure=NULL;
				return s;
			}
			else if(f->type)
				return f->type;
			else
			{
				s->u.structure=NULL;
				return s;
			}
		}
		else // STRUCT OptTag LC DefList RC
		{
			Node *deflist=root->children[0]->children[3];
			s->u.structure=NULL;
			for(;deflist;deflist=deflist->children[1]) // Def DefList
			{
				Node *def=deflist->children[0];
				Type type=specifier(def->children[0]);
				Node *declist=def->children[1];
				for(;declist->childnum==3;declist=declist->children[2])
				{
					FieldList f=vardec(declist->children[0]->children[0],type);
					if(declist->children[0]->childnum!=1) // initial in the define of struct
					{
						printf("Eror type 15 at line %d: Variable \"%s\" in struct is initialized.\n",def->line,f->name);
					}
					FieldList temp=s->u.structure;
					//while(temp)
					for(;temp;temp=temp->tail)
					{
						if(strcmp(temp->name,f->name)==0)
						{
							printf("Error type 15 at line %d: Redefined field \"%s\".\n",def->line,f->name);
							break;
						}
					}
					if(temp==NULL)
					{
						if(find(f->name,0))
							printf("Error type 3 at line %d: Redefined variable \"%s\".\n",def->line,f->name);
						else // insert hash and linked list of struct
						{
							insert(f);
							f->tail=s->u.structure;
							s->u.structure=f;
						}
					}
				}
				// last def
				FieldList f=vardec(declist->children[0]->children[0],type);
				if(declist->children[0]->childnum!=1) // initial in the define of struct
				{
					printf("Error type 15 at line %d: Variable \"%s\" in struct is initialized.\n",def->line,f->name);
				}
				FieldList temp=s->u.structure;
				//while(temp)
				for(;temp;temp=temp->tail)
				{
					if(strcmp(temp->name,f->name)==0)
					{
						printf("Error type 15 at line %d: Redefined field \"%s\".\n",def->line,f->name);
						break;
					}
				}
				if(temp==NULL)
				{
					if(find(f->name,0))
						printf("Error type 3 at line %d: Redefined variable \"%s\".\n",def->line,f->name);
					else // insert hash and linked list of struct
					{
						insert(f);
						f->tail=s->u.structure;
						s->u.structure=f;
					}
				}
			}
			if(root->children[0]->children[1]) // OptTag
			{
				FieldList f=(FieldList)malloc(sizeof(FieldList_));
				f->type=s;
				char *name=root->children[0]->children[1]->children[0]->content;
				//printf("test:%s",s)
				f->name=name;
				if(find(f->name,0))
					printf("Error type 16 at line %d: Duplicated name \"%s\".\n",root->line,f->name);
				else
					insert(f);
			}
			return s;
		}
	}
}

/*
void extdeflist(Mode *root)
{
	Node *extdeflist=root;
	for(;extdeflist->childnum!=0;extdeflist=extdeflist->children[1])
	{
		Node *extdef=extdeflist->children[0];
		Type type=specifier(extdef->children[0]);
		if(strcmp(extdef->children[1]->type,"ExtDecList")==0)
		{
			Node *temp=extdef->children[1];
*/

void extdeflist(Node *root)
{
    Node* extdeflist=root;
    while(extdeflist->childnum!=0) // ExtDef ExtDefList
	{
        Node* extdef=extdeflist->children[0];
        Type type=specifier(extdef->children[0]);
        if(strcmp(extdef->children[1]->type,"ExtDecList")==0) // Specifier ExtDecList SEMI
		{
            Node* temp=extdef->children[1]; // ExtDecList
            FieldList f;
            //while(temp->childnum==3){
			for(;temp->childnum==3;temp=temp->children[2])
			{
                f=vardec(temp->children[0],type);
                if(find(f->name,0)!=NULL)
                    printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",extdef->line,f->name);
                else insert(f);
            }
            f=vardec(temp->children[0],type);
            if(find(f->name,0)!=NULL)
                printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",extdef->line,f->name);
            else insert(f);
        }
        else if(strcmp(extdef->children[1]->type,"FunDec")==0){//Specifier FunDec CompSt
            FieldList f=(FieldList )malloc(sizeof(FieldList_));
            f->name=extdef->children[1]->children[0]->content;
            Type t=(Type)malloc(sizeof(Type_));
            t->kind=FUNCTION;
            t->u.function.funcType=type;
            //ID LP RP already done
            t->u.function.num=0;
            t->u.function.param=NULL;

            if(strcmp(extdef->children[1]->children[2]->type,"VarList")==0) //ID LP VarList RP
			{
                Node *VarList=extdef->children[1]->children[2];
                while(VarList->childnum!=1) // ParamDec COMMA VarList
				{
                    Type tempType=specifier(VarList->children[0]->children[0]);
                    FieldList tempField=vardec(VarList->children[0]->children[1],tempType);
                    if(find(tempField->name,0)!=NULL)
                        printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",extdef->line,tempField->name);
                    else insert(tempField);
                    t->u.function.num++;
                    tempField->tail=t->u.function.param;
                    t->u.function.param=tempField;

                    VarList=VarList->children[2];
                }//ParamDec
                Type tempType=specifier(VarList->children[0]->children[0]);
                FieldList tempField=vardec(VarList->children[0]->children[1],tempType);
                if(find(tempField->name,0)!=NULL)
                    printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",extdef->line,tempField->name);
                else insert(tempField);
                t->u.function.num++;
                tempField->tail=t->u.function.param;
                t->u.function.param=tempField;
            }
            f->type=t;
            if(find(f->name,1)!=NULL)
                printf("Error type 4 at Line %d: Redefined function \"%s\".\n",extdef->line,f->name);
            else insert(f);

            //CompSt->LC DefList StmtList RC
            compst(extdef->children[2],type);
        }
        else{//Specifier SIMI
            //do nothing
        }

        if(extdeflist->children[1]==NULL)//extdef
            return;
        extdeflist=extdeflist->children[1];
    }
}

void compst(Node *root,Type functype)
{
    Node *compst=root;
    deflist(compst->children[1]);
    Node *stmtlist=compst->children[2];
    while(stmtlist!=NULL){
        Node *temp=stmtlist->children[0];
        stmt(temp,functype);
        stmtlist=stmtlist->children[1];
    }
}

void deflist(Node *root)
{
    Node* deflist=root;
    for(;deflist;deflist=deflist->children[1]) //Def DefList
	{
        Node* def=deflist->children[0];
        Type type=specifier(def->children[0]);
        Node *declist=def->children[1];
        while(declist->childnum==3){//Dec COMMA DecList
            FieldList f=vardec(declist->children[0]->children[0],type);
            if(find(f->name,0)!=NULL)
                printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",declist->line,f->name);
            else insert(f);
            declist=declist->children[2];
        }
        FieldList f=vardec(declist->children[0]->children[0],type);
        if(find(f->name,0)!=NULL)
            printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",declist->line,f->name);
        else insert(f);
        if(deflist->children[1]==NULL)//Def
            return;
    }
}

void stmt(Node *root,Type functype){
    Node *temp=root;
    if(strcmp(temp->children[0]->type,"RETURN")==0) //RETURN Exp SEMI
	{
        Type returntype=exp(temp->children[1]);
        if(equal(functype,returntype)==0)
            printf("Error type 8 at Line %d: Type mismatched for return.\n",temp->line);
    }
    else if(strcmp(temp->children[0]->type,"Exp")==0) //Exp
        exp(temp->children[0]);
    else if(strcmp(temp->children[0]->type,"CompSt")==0) //CompSt
        compst(temp->children[0],functype);
    else if(strcmp(temp->children[0]->type,"WHILE")==0) //WHILE LP Exp RP Stmt
	{
        Type t=exp(temp->children[2]);
        //if(!((typ->kind==BASIC)&&(typ->u.basic== 0)))
		if(t->kind!=BASIC||t->u.basic!=0)
            printf("Error type 5 at Line %d: Only type INT could be used for judgement.\n",temp->line);
        stmt(temp->children[4],functype);
    }
    else if(temp->childnum<6) //IF LP Exp RP Stmt
	{
        Type t=exp(temp->children[2]);
        if((t!=NULL)&&(t->kind!=BASIC||t->u.basic!=0))
		{
            //if(!((typ->kind==BASIC)&&(typ->u.basic== 0)))
                printf("Error type 5 at Line %d: Only type INT could be used for judgement.\n",temp->line);
		}

        stmt(temp->children[4],functype);
    }
    else //IF LP Exp RP Stmt ELSE Stmt
	{
        Type t=exp(temp->children[2]);
        //if(!((typ->kind==BASIC)&&(typ->u.basic== 0)))
		if(t->kind!=BASIC||t->u.basic!=0)
            printf("Error type 5 at Line %d: Only type INT could be used for judgement.\n",temp->line);
        stmt(temp->children[4],functype);
        stmt(temp->children[6],functype);
    }
}

Type exp(Node* root)
{
    if(root==NULL)
        return NULL;
    else if((strcmp(root->children[0]->type,"ID")==0)&&(root->childnum==1)) //ID
	{
        FieldList f=find(root->children[0]->content,0);
        if(f!=NULL)
            return f->type;
        else
		{
            printf("Error type 1 at Line %d: Undefined variable \"%s\".\n",root->line,root->children[0]->content);
            return NULL;
        }
    }
    else if(strcmp(root->children[0]->type,"INT")==0) //INT
	{
        Type t=(Type)malloc(sizeof(Type_));
        t->kind=BASIC;
        t->u.basic=0;
        return t;
    }
    else if(strcmp(root->children[0]->type,"FLOAT")==0) //FLOAT
	{
        Type t=(Type)malloc(sizeof(Type_));
        t->kind=BASIC;
        t->u.basic=1;
        return t;
    }
    else if((strcmp(root->children[0]->type,"LP")==0)||(strcmp(root->children[0]->type,"MINUS")==0)||(strcmp(root->children[0]->type,"NOT")==0))
	{
        return exp(root->children[1]);
    }
    else if((strcmp(root->children[1]->type,"PLUS")==0)||(strcmp(root->children[1]->type,"MINUS")==0)||(strcmp(root->children[1]->type,"STAR")==0)||(strcmp(root->children[1]->type,"DIV")==0))
	{
        Type t1=exp(root->children[0]);
        Type t2=exp(root->children[2]);
        if(equal(t1,t2)==0)
		{
            if((t1!=NULL)&&(t2!=NULL))
                printf("Error type 7 at Line %d: Type mismatched for operands.\n",root->line);
            return NULL;
        }
        else return t1;
    }
    else if((strcmp(root->children[1]->type,"AND")==0)||(strcmp(root->children[1]->type,"OR")==0)||(strcmp(root->children[1]->type,"RELOP")==0))
	{
        Type t1=exp(root->children[0]);
        Type t2=exp(root->children[2]);
        if(equal(t1,t2)==0)
		{
            if((t1!=NULL)&&(t2!=NULL))
                printf("Error type 7 at Line %d: Type mismatched for operands.\n",root->line);
            return NULL;
        }
        else
		{
			Type t=(Type)malloc(sizeof(Type_));
			t->kind=BASIC;
			t->u.basic=0;
			return t;
		}
    }
    else if(strcmp(root->children[1]->type,"ASSIGNOP")==0)
	{
        if(root->children[0]->childnum==1)
		{
            if(strcmp(root->children[0]->children[0]->type,"ID")!=0)
			{
                printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n",root->line);
                return NULL;
            }
        }
        else if(root->children[0]->childnum==3)
		{
            if((strcmp(root->children[0]->children[0]->type,"Exp")!=0)||(strcmp(root->children[0]->children[1]->type,"DOT")!=0)||(strcmp(root->children[0]->children[2]->type,"ID")!=0))
			{
                printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n",root->line);
                return NULL;
            }
        }
        else if(root->children[0]->childnum==4)
		{
            if((strcmp(root->children[0]->children[0]->type,"Exp")!=0)||(strcmp(root->children[0]->children[1]->type,"LB")!=0)||(strcmp(root->children[0]->children[2]->type,"Exp")!=0)||(strcmp(root->children[0]->children[3]->type,"RB")!=0))
			{
                printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n",root->line);
                return NULL;
            }
        }
        Type t1=exp(root->children[0]);
        Type t2=exp(root->children[2]);
        if(equal(t1,t2)==0){
            if((t1!=NULL)&&(t2!=NULL))
                printf("Error type 5 at Line %d: Type mismatched for assignment.\n",root->line);
            return NULL;
        }
        else return t1;
    }
    else if(strcmp(root->children[0]->type,"ID")==0) //ID LP RP
	{
        FieldList f1=find(root->children[0]->content,1);
        if(!f1)
		{
            FieldList f2=find(root->children[0]->content,0);
            if(f2)
                printf("Error type 11 at Line %d: \"%s\" is not a function.\n",root->line,root->children[0]->content);
            else 
				printf("Error type 2 at Line %d: Undefined function \"%s\".\n",root->line,root->children[0]->content);
            return NULL;
        }
        Type definedtype=f1->type;

        Type t=(Type)malloc(sizeof(Type_));
        t->kind=FUNCTION;
        t->u.function.num=0;
        t->u.function.param=NULL;
        if(strcmp(root->children[2]->type,"RP")!=0) //ID LP Args RP
		{
            Node* temp=root->children[2];
            for(;temp->childnum!=1;temp=temp->children[2]) //Exp COMMA Args
			{
                Type temptype=exp(temp->children[0]);
                FieldList tempfield=(FieldList )malloc(sizeof(FieldList_));
                tempfield->name="no";
				tempfield->type=temptype;
                t->u.function.num++;
                tempfield->tail=t->u.function.param;
                t->u.function.param=tempfield;
            }//Exp
            Type temptype=exp(temp->children[0]);
            FieldList tempfield=(FieldList )malloc(sizeof(FieldList_));
            tempfield->name="no";//just for temp compare
	    tempfield->type=temptype;
            t->u.function.num++;
            tempfield->tail=t->u.function.param;
            t->u.function.param=tempfield;
        }
        if(equal(t,definedtype)==0)
		{
            printf("Error type 9 at Line %d: Params wrong in function \"%s\".\n",root->line,root->children[0]->content);
            return NULL;
        }
        else return definedtype->u.function.funcType;
    }
    else if(strcmp(root->children[1]->type,"DOT")==0) //Exp DOT ID
	{
        Type t1=exp(root->children[0]);
        if(t1->kind!=STRUCTURE)
		{
            Node* temp=root->children[0];
            char *s;
            //switch(temp->childnum){
              //  case 1:{
			if(temp->childnum==1)
			{
                if(strcmp(temp->children[0]->type,"ID")==0)
                        s=temp->children[0]->content;
            }
			else if(temp->childnum==3)
			{
                if(strcmp(temp->children[2]->type,"ID")==0)
                        s=temp->children[0]->content;
            }
			else if(temp->childnum==4)
			{
                if(strcmp(temp->children[0]->type,"Exp")==0)
                    if(strcmp(temp->children[0]->children[0]->type,"ID")==0)
                        s=temp->children[0]->children[0]->content;
            }
			else
                s="error";
            if(find(s,0)!=NULL)
                printf("Error type 13 at Line %d: Illegal use of \".\".\n",root->line);
            return NULL;
        }
        char *s=root->children[2]->content;
        FieldList temp=t1->u.structure;
        while(temp!=NULL){
            if(strcmp(temp->name,s)==0)
                return temp->type;

            temp=temp->tail;
        }
        
        printf("Error type 14 at Line %d: Non-existent field \"%s\".\n",root->line,root->children[2]->content);
        return NULL;
    }
    else if(strcmp(root->children[1]->type,"LB")==0) //Exp LB Exp RB
	{
        Type t1=exp(root->children[0]);
        if(t1->kind!=ARRAY)
		{
            Node* temp=root->children[0];
            char *s;
			if(temp->childnum==1)
			{
                if(strcmp(temp->children[0]->type,"ID")==0)
                    s=temp->children[0]->content;
			}
			else if(temp->childnum==3)
			{
                if(strcmp(temp->children[2]->type,"ID")==0)
                    s=temp->children[0]->content;
            }
            else if(temp->childnum==4)
			{
                if(strcmp(temp->children[0]->type,"Exp")==0)
                    if(strcmp(temp->children[0]->children[0]->type,"ID")==0)
                        s=temp->children[0]->children[0]->content;
            }
			else
                s="error";
            if(find(s,0)!=NULL)
                printf("Error type 10 at Line %d: \"%s\" is not an array.\n",root->line,s);
            return NULL;
        }
        Type temp=exp(root->children[2]);
        if(temp->kind!=BASIC)
		{
            printf("Error type 12 at Line %d: there is not a integer between \"[\" and \"]\".\n",root->line);
            return NULL;
        }
        else if(temp->u.basic==1)
		{
			printf("Error type 12 at Line %d: \"%s\" is not an integer.\n",root->line,root->children[2]->children[0]->content);
            return NULL;
        }
        //no error
        return t1->u.array.elem;
    }
    else{
        printf("in\n");
        return NULL;
    }
}

