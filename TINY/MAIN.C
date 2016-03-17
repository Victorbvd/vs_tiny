/****************************************************/
/* File: main.c                                     */
/* Main program for TINY compiler                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE FALSE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE FALSE


/* set NO_CODE to TRUE to get a compiler that does not
 * generate code
 */
#define NO_CODE FALSE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#if !NO_ANALYZE
#include "analyze.h"
#if !NO_CODE
#include "cgen.h"
#endif
#endif
#endif

/* allocate global variables */
int lineno = 0;
FILE * source;
FILE * listing;
FILE * code;

/* allocate and set tracing flags */
int EchoSource = TRUE;
int TraceScan = TRUE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = TRUE;

int Error = FALSE;

main(int argc, char * argv[])
{
	//	char* pgm = "D:\\code\\Visual Studio\\visual studio 2015\\Projects\\TINY\\tiny\\Debug\\SAMPLE.TNY";

	TreeNode * syntaxTree;
	char pgm[120]; /* source code file name */

	if (argc != 2)
	{
		fprintf(stderr, "usage: %s <filename>\n", argv[0]);
		exit(1);
	}
	strcpy(pgm, argv[1]);
	if (strchr(pgm, '.') == NULL)
		strcat(pgm, ".tny");
	//char* pgm = "D:\\code\\Visual Studio\\visual studio 2015\\Projects\\TINY\\tiny\\Debug\\SAMPLE.TNY";


	source = fopen(pgm, "r");
	if (source == NULL)
	{
		fprintf(stderr, "File %s not found\n", pgm);
		exit(1);
	}
	listing = stdout; /* send listing to screen */
	fprintf(listing, "\nTINY COMPILATION: %s\n", pgm);
#if NO_PARSE
	while (getToken() != ENDFILE);
#else
	syntaxTree = parse();//first 词法语法分析，构造语法树
	if (TraceParse) {
		fprintf(listing, "\nSyntax tree:\n");
		printTree(syntaxTree);
		printf("语法树OK\n");
	}
#if !NO_ANALYZE
	printf("Error?\n");
	if (!Error)
	{
		printf("no error\n");
		if (TraceAnalyze) fprintf(listing, "\nBuilding Symbol Table...\n");
		buildSymtab(syntaxTree);//语义分析，构造符号表
		if (TraceAnalyze) fprintf(listing, "\nChecking Types...\n");
		typeCheck(syntaxTree);//语义分析，执行类型检查
		if (TraceAnalyze) fprintf(listing, "\nType Checking Finished\n");
	}
	else
		printf("it had error\n");
#if !NO_CODE
	printf("Error?\n");
	if (!Error)
	{
		printf("no error\n");
		char * codefile;
		int fnlen = strcspn(pgm, ".");
		codefile = (char *)calloc(fnlen + 4, sizeof(char));
		strncpy(codefile, pgm, fnlen);
		strcat(codefile, ".tm");
		code = fopen(codefile, "w");
		if (code == NULL)
		{
			printf("Unable to open %s\n", codefile);
			exit(1);
		}
		codeGen(syntaxTree, codefile);//完成代码生成
		fclose(code);
	}
#endif
#endif
#endif
	fclose(source);
	return 0;
}

