/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode * program(void);
static TreeNode * declarations(void);

static TreeNode * stmt_sequence(void);
static TreeNode * statement(void);
static TreeNode * if_stmt(void);
static TreeNode * repeat_stmt(void);
static TreeNode * assign_stmt(void);
static TreeNode * read_stmt(void);
static TreeNode * write_stmt(void);
static TreeNode * while_stmt(void);


/*decalrations()_____function____define__*/
static TreeNode *decl(void);
static TreeNode *type_specifier(void);
static TreeNode *varlist(void);
/*______exp()_____function____define__*/
static TreeNode * exp(void);
static TreeNode *arithmetic_exp(void);
static TreeNode * term(void);
static TreeNode * factor(void);
/*______bool_exp()_____function____define__*/
static TreeNode *bool_exp(void);
static TreeNode *comparison_exp(void);
static TreeNode * bterm(void);
static TreeNode * bfactor(void);
/*______string_exp()_____function____define__*/
static TreeNode *string_exp(void);



static void syntaxError(char * message)
{
	fprintf(listing, "\n>>> ");
	fprintf(listing, "Syntax error at line %d: %s", lineno, message);
	Error = TRUE;
}

static void match(TokenType expected)
{
	if (token == expected)
		token = getToken();
	else {
		syntaxError("unexpected token ---match---> ");
		printToken(token, tokenString);
		fprintf(listing, "      ");
	}
}
TreeNode * program(void)
{ 
	TreeNode * t = declarations();
	TreeNode * t_tmp = t;
	while (t_tmp->sibling != NULL)
		t_tmp = t_tmp->sibling;
	if (t_tmp != NULL) t_tmp->sibling = stmt_sequence();
	t_tmp = NULL;
	return t;
}

//原本EBNF定义为declarations-> decl;declarations|$，应该递归查找
//现在修改下，declarations->(decl;)*,这样在一个while循环中即可
TreeNode * declarations(void)
{
	TreeNode * t = decl();
	match(SEMI);
	TreeNode * p = t;
	while ((token != ENDFILE) && (token != END))
	{
		TreeNode * q = NULL;
		q = decl();
		if (q != NULL) {
			if (t == NULL) t = p = q;
			else /* now p cannot be NULL either */
			{
				p->sibling = q;
				p = q;
			}
			match(SEMI);
		}
		else {
			break;
		}
	}
	return t;
}

TreeNode * decl(void)
{

	TreeNode * t = type_specifier();
	if (t != NULL) {
		ExpType t_type_tmp = t->type;
		t->child[0] = varlist(t_type_tmp);
	}
	return t;
}
TreeNode * type_specifier(void)
{
	TreeNode * t = NULL;
	switch (token)
	{
	case INT:
		t = newDefineNode(IntD);
		t->attr.name = "int";
		t->type = Integer;
		match(INT);

		/*match(INT);
		if ((t != NULL) && (token == ID))
			t->attr.name = copyString(tokenString);
		match(ID);*/

		break;
	case BOOL:
		t = newDefineNode(BoolD);
		t->attr.name = "bool";

		t->type = Boolean;
		match(BOOL);
		/*match(BOOL);
		if ((t != NULL) && (token == ID))
			t->attr.name = copyString(tokenString);
		match(ID);*/

		break;
	case CHAR:
		t = newDefineNode(CharD);
		t->attr.name = "char";
		t->type = Char;
		match(CHAR);
		/*match(CHAR);
		if ((t != NULL) && (token == ID))
			t->attr.name = copyString(tokenString);
		match(ID);*/
		break;
	default:
		/*syntaxError("unexpected token -> ");
		printToken(token, tokenString);
		token = getToken();

		t->attr.name = "bool";
		match(BOOL);
		
		*/
		break;
	}
	return t;
}

TreeNode * varlist(ExpType t_type)
{
	TreeNode * t = NULL;
	if (token == ID)
	{
		t = newExpNode(IdK);
		t->attr.name = copyString(tokenString);
		t->type = t_type;
		match(ID);
	}

	TreeNode * p = t;

	while ((token == COMMA) && (token != ENDFILE) && (token != END))
	{
		TreeNode * q;
		match(COMMA);
		q = newExpNode(IdK);
		q->attr.name = copyString(tokenString);
		t->type = t_type;
		match(ID);
		if (q != NULL) {
			if (t == NULL) t = p = q;
			else /* now p cannot be NULL either */
			{
				p->sibling = q;
				p = q;
			}
		}
	}
	return t;
}
 
TreeNode * stmt_sequence(void)
{
	TreeNode * t = statement();
	TreeNode * p = t;
	while ((token != ENDFILE) && (token != END) &&
		(token != ELSE) && (token != UNTIL))
	{
		TreeNode * q;
		match(SEMI);
		q = statement();
		if (q != NULL) {
			if (t == NULL) t = p = q;
			else /* now p cannot be NULL either */
			{
				p->sibling = q;
				p = q;
			}
		}
	}
	return t;
}

TreeNode * statement(void)
{
	TreeNode * t = NULL;
	switch (token) {
	case IF: t = if_stmt(); break;
	case REPEAT: t = repeat_stmt(); break;
	case ID: t = assign_stmt(); break;
	case READ: t = read_stmt(); break;
	case WRITE: t = write_stmt(); break;
		/*add from nango*/
	case WHILE: t = while_stmt(); break;

	default:
		syntaxError("unexpected token ---statement-> ");
		printToken(token, tokenString);
		token = getToken();
		break;
	} /* end case */
	return t;
}

/*add from nango*/
//statment->if_stmt|repeat_stmt|assign_stmt|read_stmt|write_stmt|while_stmt

//while 语句块语法分析
TreeNode * while_stmt()
{
	TreeNode * t = newStmtNode(WhileK);
	match(WHILE);
	if (t != NULL) t->child[0] = bool_exp();
	match(DO);
	if (t != NULL) t->child[1] = stmt_sequence();
	match(END);
	return t;
}

TreeNode * if_stmt(void)
{
	TreeNode * t = newStmtNode(IfK);
	match(IF);
	if (t != NULL) t->child[0] = bool_exp();
	match(THEN);
	if (t != NULL) t->child[1] = stmt_sequence();
	if (token == ELSE) {
		match(ELSE);
		if (t != NULL) t->child[2] = stmt_sequence();
	}
	match(END);
	return t;
}

TreeNode * repeat_stmt(void)
{
	TreeNode * t = newStmtNode(RepeatK);
	match(REPEAT);
	if (t != NULL) t->child[0] = stmt_sequence();
	match(UNTIL);
	if (t != NULL) t->child[1] = exp();
	return t;
}

TreeNode * assign_stmt(void)
{
	TreeNode * t = newStmtNode(AssignK);
	if ((t != NULL) && (token == ID))
		t->attr.name = copyString(tokenString);
	match(ID);
	match(ASSIGN);
	if (t != NULL) t->child[0] = exp();
	return t;
}

TreeNode * read_stmt(void)
{
	TreeNode * t = newStmtNode(ReadK);
	match(READ);
	if ((t != NULL) && (token == ID))
		t->attr.name = copyString(tokenString);
	match(ID);
	return t;
}

TreeNode * write_stmt(void)
{
	TreeNode * t = newStmtNode(WriteK);
	match(WRITE);
	if (t != NULL) t->child[0] = exp();
	return t;
}


/*changed from nango*/
 

TreeNode * exp(void)
{
	TreeNode * t = NULL;
	switch (token)
	{
	case STR:
		t = string_exp();
		break;
	default:
		t = bool_exp();
		break;
	}
	return t;
}



//识别一条算术比较式，包括> < >= <=  如(a+b)>=(c+d)
TreeNode * comparison_exp(void)
{
	TreeNode * t = arithmetic_exp();
	if ((token == LT) || (token == RT) || (token == EQ) || (token == LTEQ) || (token == RTEQ)) {
		TreeNode * p = newExpNode(OpK);
		if (p != NULL) {
			p->child[0] = t;
			p->attr.op = token;
			t = p;
		}
		match(token);
		if (t != NULL)
			t->child[1] = arithmetic_exp();
	}
	return t;
}
//识别一条算术表达式如 a+b， a-b
TreeNode * arithmetic_exp(void)
{
	TreeNode * t = term();
	while ((token == PLUS) || (token == MINUS))
	{
		TreeNode * p = newExpNode(OpK);
		if (p != NULL) {
			p->child[0] = t;
			p->attr.op = token;
			t = p;
			match(token);
			t->child[1] = term();
		}
	}
	return t;
}

//识别一条bool运算，其中包括识别 a >= b+c or e>f and h<=j   也识别 a>=b+c 
TreeNode * bool_exp(void)
{
	TreeNode * t = bterm();
	while ((token == OR))
	{
		//TreeNode * p = newExpNode(OpK);
		TreeNode * p = newExpNode(LogicOpK);
		if (p != NULL) {
			p->child[0] = t;
			p->attr.op = token;
			t = p;
			match(token);
			t->child[1] = bterm();
		}
	}
	return t;
}
 
//识别一串字符串，但是不能识别 'abc' and 'efg' 
//还没添加（实验不要求。不过做法和bool_exp一样，重写写下EBNF即可
TreeNode * string_exp(void)
{

	TreeNode * t = NULL;
	t = newExpNode(StringK);
	if ((t != NULL) && (token == STR))
		t->attr.name = copyString(tokenString);
	match(STR);
	return t;
}
TreeNode * term(void)
{
	TreeNode * t = factor();
	while ((token == TIMES) || (token == OVER))
	{
		TreeNode * p = newExpNode(OpK);
		if (p != NULL) {
			p->child[0] = t;
			p->attr.op = token;
			t = p;
			match(token);
			p->child[1] = factor();
		}
	}
	return t;
}

TreeNode * bterm(void)
{
	TreeNode * t = bfactor();
	while (token == AND)
	{
		TreeNode * p = newExpNode(LogicOpK);
		if (p != NULL) {
			p->child[0] = t;
			p->attr.op = token;
			t = p;
			match(token);
			p->child[1] = bfactor();
		}
	}
	return t;
}

TreeNode * factor(void)
{
	TreeNode * t = NULL;
	switch (token) {
	case NUM:
		t = newExpNode(ConstK);
		if ((t != NULL) && (token == NUM))
		{
			t->attr.val = atoi(tokenString);
			t->type = Integer;
		}
			match(NUM);
		break;
	case ID:
		t = newExpNode(IdK);
		if ((t != NULL) && (token == ID))
			t->attr.name = copyString(tokenString);
		match(ID);
		break;
	case LPAREN:
		match(LPAREN);
		t = exp();
		match(RPAREN);
		break;
	default:
		syntaxError("unexpected token ------factor---> ");
		printToken(token, tokenString);
		token = getToken();
		break;
	}
	return t;
}
 

TreeNode * bfactor(void)
{
	return comparison_exp();
}
/*	 
	TreeNode * t = arithmetic_exp();//如果是算术运算则继续往下 
	if ((token == LT) || (token == RT) || (token == LTEQ) || (token == RTEQ)||(token == EQ)) {
		TreeNode * p = newExpNode(OpK);
		if (p != NULL) {
			p->child[0] = t;
			p->attr.op = token;
			t = p;
		}
		match(token);
		if (t != NULL)
			t->child[1] = arithmetic_exp();
	}  
	return t;
}*/


/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly 
 * constructed syntax tree
 */
TreeNode * parse(void)
{
	TreeNode * t;
	token = getToken();
	t = program();
	if (token != ENDFILE)
		syntaxError("Code ends before file\n");
	return t;
}
