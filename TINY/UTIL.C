/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
void printToken(TokenType token, const char* tokenString)
{
	switch (token)
	{
	case IF:fprintf(listing, "(KEY,if)\n"); break;
	case THEN:fprintf(listing, "(KEY,then)\n"); break;
	case ELSE:fprintf(listing, "(KEY,else)\n"); break;
	case END:fprintf(listing, "(KEY,end)\n"); break;
	case REPEAT:fprintf(listing, "(KEY,repeat)\n"); break;
	case UNTIL:fprintf(listing, "(KEY,until)\n"); break;
	case READ:fprintf(listing, "(KEY,read)\n"); break;
	case WRITE:
		fprintf(listing, "(KEY,write: %s\n", tokenString); break;

	case WHILE: fprintf(listing, "(KEY,while)\n"); break;


		//	fprintf(listing,
			//	"reserved word: %s\n", tokenString);
			//break;
	case ASSIGN: fprintf(listing, "(SYM,:=)\n"); break;
	case LT: fprintf(listing, "(SYM,<)\n"); break;

		/*****  nango  START */
	case RT: fprintf(listing, "(SYM,>)\n"); break;
	case LTEQ: fprintf(listing, "(SYM,<=)\n"); break;
	case RTEQ: fprintf(listing, "(SYM,>=)\n"); break;

	case OR: fprintf(listing, "(KEY,or)\n"); break;
	case AND: fprintf(listing, "(KEY,and)\n"); break;
	case INT: fprintf(listing, "(KEY,int)\n"); break;
	case BOOL: fprintf(listing, "(KEY,bool)\n"); break;
	case CHAR: fprintf(listing, "(KEY,char)\n"); break;
	case COMMA: fprintf(listing, "(SYM,,)\n"); break;
	case STR:
		fprintf(listing,
			"(STR, string: %s)\n", tokenString);
		break;
	case DO:fprintf(listing, "(KEY,do)\n"); break;

		/********nango end***************/
	case EQ: fprintf(listing, "(SYM,=)\n"); break;
	case LPAREN: fprintf(listing, "(SYM,()\n"); break;
	case RPAREN: fprintf(listing, "(SYM,))\n"); break;
	case SEMI: fprintf(listing, "(SYM,;)\n"); break;
	case PLUS: fprintf(listing, "(SYM,+)\n"); break;
	case MINUS: fprintf(listing, "(SYM,-)\n"); break;
	case TIMES: fprintf(listing, "(SYM,*)\n"); break;
	case OVER: fprintf(listing, "(SYM,/)\n"); break;
	case ENDFILE: fprintf(listing, "EOF\n"); break;
	case NUM:
		fprintf(listing,
			"(NUM, %s)\n", tokenString);
		break;
	case ID:
		fprintf(listing,
			"(ID, %s)\n", tokenString);
		break;
	case ERROR:
		fprintf(listing,
			"ERROR: %s\n", tokenString);
		break;
	default: /* should never happen */
		fprintf(listing, "Unknown token: %d\n", token);
	}
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 *取语句种类作为参数，创建一个相应类型的语句结点
 */
TreeNode * newStmtNode(StmtKind kind)
{
	TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);
	else {
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
		t->sibling = NULL;
		t->nodekind = StmtK;
		t->kind.stmt = kind;
		t->lineno = lineno;
	}
	return t;
}

/**取exp的类型座位参数，创建一个相应类型的表达式结点 
 *Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode * newExpNode(ExpKind kind)
{
	TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);
	else {
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
		t->sibling = NULL;
		t->nodekind = ExpK;
		t->kind.exp = kind;
		t->lineno = lineno;
		t->type = Void;
	}
	return t;
}
TreeNode * newDefineNode(DefineKind kind)
{
	TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
	int i;
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);
	else {
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;
		t->sibling = NULL;
		t->nodekind = Definek;
		t->kind.define = kind;
		t->lineno = lineno;
	}
	return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s)
{
	int n;
	char * t;
	if (s == NULL) return NULL;
	n = strlen(s) + 1;
	t = malloc(n);
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);
	else strcpy(t, s);
	return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{
	int i;
	for (i = 0; i < indentno; i++)
		fprintf(listing, " ");
}

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 *输出语法树，但语法树仅被打印为一种线性的表示
 */
void printTree(TreeNode * tree)
{
	int i;
	INDENT;
	while (tree != NULL) {
		printSpaces();
		if (tree->nodekind == StmtK)
		{
			switch (tree->kind.stmt) {
			case IfK:
				fprintf(listing, "If\n");
				break;
			case RepeatK:
				fprintf(listing, "Repeat\n");
				break;
			case AssignK:
				fprintf(listing, "Assign to: %s\n", tree->attr.name);
				break;
			case ReadK:
				fprintf(listing, "Read: %s\n", tree->attr.name);
				break;
			case WriteK:
				fprintf(listing, "Write\n");
				break;
			case WhileK:
				fprintf(listing, "While\n");
				break;
			default:
				fprintf(listing, "Unknown StmtKNode kind\n");
				break;
			}
		}
		
		else if (tree->nodekind == ExpK)
		{
			switch (tree->kind.exp) {
			case OpK:
				fprintf(listing, "Op: ");
				printToken(tree->attr.op, "\0");
				break;
			case ConstK:
				fprintf(listing, "Const: %d\n", tree->attr.val);
				break;
			case IdK:
				fprintf(listing, "Id: %s\n", tree->attr.name);
				break;
			case StringK:
				fprintf(listing, "STR: %s\n", tree->attr.name);
				break;
			case LogicOpK:
				fprintf(listing, "LogicOp: ");
				printToken(tree->attr.op, "\0");
				break;
			default:
				fprintf(listing, "Unknown ExpKNode kind\n");
				break;
			}
		}
		else if (tree->nodekind == Definek)
		{
			int  flag_tmp = 0;
			switch (tree->kind.define) {
			case IntD:
				fprintf(listing, "Int: %s\n", tree->attr.name);
				flag_tmp = 1;
				break;
			case CharD:
				fprintf(listing, "Char: %s\n", tree->attr.name);
				flag_tmp = 1;
				break;
			case BoolD:
				fprintf(listing, "Bool: %s\n", tree->attr.name);
				flag_tmp = 1;
				break;
			default:
				flag_tmp = 0;
				fprintf(listing, "Unknown DefineKNode kind\n");
				break;
			}
			 if (flag_tmp == 1)
			{

				//fprintf(listing, "%s\n", tree->attr.name);
				/*TreeNode *child_tmp = tree->child[0];
				while (child_tmp != NULL)
				{
					fprintf(listing,"\t%s,", child_tmp->attr.name);
					child_tmp =  child_tmp->sibling;
				}
				child_tmp = NULL;
				fprintf(listing,"\n");*/
			} 
		}
		else fprintf(listing, "Unknown node kind\n");
		for (i = 0; i < MAXCHILDREN; i++)
			printTree(tree->child[i]);
		tree = tree->sibling;
	}
	UNINDENT;
}
