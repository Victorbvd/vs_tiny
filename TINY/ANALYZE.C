/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"

/* counter for variable memory locations */
static int location = 0;

/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
static void traverse(TreeNode * t,
	void(*preProc) (TreeNode *),
	void(*postProc) (TreeNode *))
{
	if (t != NULL)
	{
		preProc(t);
		{ int i;
		for (i = 0; i < MAXCHILDREN; i++)
			traverse(t->child[i], preProc, postProc);
		}
		postProc(t);
		traverse(t->sibling, preProc, postProc);
	}
}
/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t)
{
	if (t == NULL) return;
	else return;
}

static void typeError(TreeNode * t, char * message)
{
	fprintf(listing, "Type error at line %d: %s\n", t->lineno, message);
	Error = TRUE;
}

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
static void insertNode(TreeNode * t)
{
	TreeNode * p_tmp = t;
	int exp_type_tmp = -1;
	switch (t->nodekind)
	{
	case StmtK:
		switch (t->kind.stmt)//IfK,RepeatK,AssignK,ReadK,WriteK,WhileK
		{
		case IfK:
			break;
		case RepeatK:
			break;
		case AssignK: 

			exp_type_tmp = st_lookup_type(t->attr.name);
			if (exp_type_tmp == -1)
				/* not yet in table, so treat as new definition */
			{
				//st_insert(t->attr.name, t->lineno, location++,t->type);
				printf("error!!!!---name:%s,line:%d\n", p_tmp->attr.name, p_tmp->lineno);
			}
			else
				/* already in table, so ignore location,
				add line number of use only */
			{
			  st_insert(t->attr.name, t->lineno, 0, exp_type_tmp);
			  t->type = exp_type_tmp;
			}
				break;
		case ReadK:

			exp_type_tmp = st_lookup_type(t->attr.name);
			if (exp_type_tmp == -1)
				/* not yet in table, so treat as new definition */
			{
				//st_insert(t->attr.name, t->lineno, location++);
				printf("error!!!!---name:%s,line:%d\n", p_tmp->attr.name, p_tmp->lineno);
			}
			else
				/* already in table, so ignore location,
				   add line number of use only */
			{
				st_insert(t->attr.name, t->lineno, 0, exp_type_tmp);
				t->type = exp_type_tmp;
			}
				break;
		case WriteK:
			break;
		case WhileK:
			break;
		default:
			break;
		}
		break;
	case ExpK://OpK, ConstK, IdK, BoolK, StringK, ArithmeticK
		switch (t->kind.exp)
		{
		case OpK:


			break;
		case ConstK:


			break;
		case IdK:
			exp_type_tmp = st_lookup_type(t->attr.name);
			if (exp_type_tmp == -1)
				/* not yet in table, so treat as new definition */
			{
				//st_insert(t->attr.name, t->lineno, location++, t->type);
				printf("error!!!!---name:%s,line:%d,__%d\n", p_tmp->attr.name, p_tmp->lineno,p_tmp->type);
			}
			else
				/* already in table, so ignore location,
				add line number of use only */
			{
				  st_insert(t->attr.name, t->lineno, 0, exp_type_tmp);
				  t->type = exp_type_tmp;
			//	printf(" name:%s,line:%d,__%d\n", p_tmp->attr.name, p_tmp->lineno, p_tmp->type);
			}
			break;
		case BoolK:
			break;
		case StringK:
			break;
		case ArithmeticK:
			break;
		case LogicOpK:
			//t->type = t->child[0]->type;
			//if(t->child[0]->type!=BOOL||t->child[1]->type!=BOOL)
			break;
		default:
			break;
		}
		break;

	case Definek:
		
		switch (t->kind.define)
		{
		case IntD:
			p_tmp = t;
			ExpType now_type = p_tmp->type;
			p_tmp = p_tmp->child[0];
			while (p_tmp != NULL) {
				if (st_lookup_type(p_tmp->attr.name) == -1)
					/* not yet in table, so treat as new definition */
				{
					st_insert(p_tmp->attr.name, p_tmp->lineno, location++, now_type);
					p_tmp->type = now_type;
				}
				else
					/* already in table, so ignore location,
					add line number of use only */
				{
					typeError(t, "error! this varial had already defined!");
					//st_insert(p_tmp->attr.name, p_tmp->lineno, 0, Void);
				}

				p_tmp = p_tmp->sibling;
			} 
			break;
		case BoolD:
			p_tmp = t;
			ExpType now_type2 = p_tmp->type;
			p_tmp = p_tmp->child[0];
			while (p_tmp != NULL) {
				if (st_lookup_type(p_tmp->attr.name) == -1)
					/* not yet in table, so treat as new definition */
				{
					st_insert(p_tmp->attr.name, p_tmp->lineno, location++,now_type2);
					p_tmp->type = now_type2;
				}
				else
					/* already in table, so ignore location,
					add line number of use only */
				{
					typeError(t, "error double define!");
					//st_insert(p_tmp->attr.name, p_tmp->lineno, 0, Void);
				}

				p_tmp = p_tmp->sibling;
			}

			break;
		case CharD:
			p_tmp = t;
			ExpType now_type3 = p_tmp->type;
			p_tmp = p_tmp->child[0];
			while (p_tmp != NULL) {
				if (st_lookup_type(p_tmp->attr.name) == -1)
					/* not yet in table, so treat as new definition */
				{
					st_insert(p_tmp->attr.name, p_tmp->lineno, location++,now_type3);
					p_tmp->type = now_type3;
				}
				else
					/* already in table, so ignore location,
					add line number of use only */ 
				{
						typeError(t, "error double define!");
						//st_insert(p_tmp->attr.name, p_tmp->lineno, 0, Void);
				}

				p_tmp = p_tmp->sibling;
			}
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree)
{
	traverse(syntaxTree, insertNode, nullProc);
	if (TraceAnalyze)
	{
		fprintf(listing, "\nSymbol table:\n\n");
		printSymTab(listing);
	}
}


/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode * t)
{
	switch (t->nodekind)
	{
	case ExpK:
		switch (t->kind.exp)//OpK, ConstK, IdK, BoolK, StringK, ArithmeticK
		{
		case OpK:
			if ((t->child[0]->type != Integer) ||
				(t->child[1]->type != Integer))
			{
				typeError(t, "Op applied to non-integer");
			}
			if ((t->attr.op == EQ) || (t->attr.op == LT) || (t->attr.op == LTEQ) || (t->attr.op == RT) || (t->attr.op == RTEQ))
				t->type = Boolean;
			else
				t->type = Integer;
			break;
		case ConstK://Êý×Ö
			//printf("constK__%d\n",t->lineno); 
			break;
		case IdK:
			if (st_lookup_type(t->attr.name) == -1)
			{
				printf("%d  %s   ", t->lineno,t->attr.name);
				typeError(t, "ID type Error___");
			}
			break;
		case LogicOpK:
			if (t->child[0]->type == Boolean&&t->child[1]->type == Boolean)
			{
				t->type = Boolean;
			}
			else
			{
				typeError(t, "LogicOpK two sides had error occurred:not boolead");
			}
			break;
		default:
			break;
		}
		break;
	case StmtK:
		switch (t->kind.stmt)
		{
		case IfK:
			if (t->child[0]->type !=Boolean)
				typeError(t->child[0], "if [0] is not Boolean");
			break;
		case AssignK:
			if(t->child[0]->type != t->type)
				typeError(t, "assignment of not the same type");
/*
			if (t->child[0]->type != Integer)
				typeError(t->child[0], "assignment of non-integer value");*/
			break;
		case WriteK:
			if (t->child[0]->type != Integer)
				typeError(t->child[0], "write of non-integer value");
			break;
			
		/*case RepeatK:
			if (t->child[1]->type == Integer)
				typeError(t->child[1], "repeat test is not Boolean");
			break;
		case ReadK:
			if (!(t->type == Integer|| t->type == Char))
				typeError(t->child[1], "reaed test is wrong not integer and char");
			break;*/
		case WhileK:
			if (t->child[0]->type != Boolean && t->child[0]->type != Integer)
			{ 
				typeError(t->child[0], "while [0] is not Boolean and not Integer");
			}
			break;
		default:
			break;
		}
		break;
	case Definek: 
		break;

	}
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree)
{
	traverse(syntaxTree, nullProc, checkNode);

}
