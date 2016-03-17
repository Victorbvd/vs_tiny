/****************************************************/
/* File: tm.c                                       */
/* The TM ("Tiny Machine") computer                 */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/******* const *******/
#define   IADDR_SIZE  1024 /* increase for large programs  指令存储区大小 */
#define   DADDR_SIZE  4096 /* increase for large programs 数据区大小*/
#define   NO_REGS 8    //寄存器数量
#define   PC_REG  7   //程序寄存器，只有这一个为专用寄存器

#define   LINESIZE  121
#define   WORDSIZE  20

/******* type  *******/

typedef enum {
	opclRR,     /* reg operands r,s,t 程序寄存器，只有这一个为专用寄存器*/
	opclRM,     /* reg r, mem d+s 寄存器&内存操作*/
	opclRA      /* reg r, int d+s 寄存器&地址操作*/
} OPCLASS;

typedef enum {
	/* RR instructions */
	opHALT,    /* RR     halt, operands are ignored */
	opIN,      /* RR     read into reg(r); s and t are ignored */
	opOUT,     /* RR     write from reg(r), s and t are ignored */
	opADD,    /* RR     reg(r) = reg(s)+reg(t) */
	opSUB,    /* RR     reg(r) = reg(s)-reg(t) */
	opMUL,    /* RR     reg(r) = reg(s)*reg(t) */
	opDIV,    /* RR     reg(r) = reg(s)/reg(t) */
	opRRLim,   /* limit of RR opcodes */

			   /* RM instructions */
	opLD,      /* RM     reg(r) = mem(d+reg(s)) */
	opST,      /* RM     mem(d+reg(s)) = reg(r) */
	opRMLim,   /* Limit of RM opcodes */

			   /* RA instructions */
	opLDA,     /* RA     reg(r) = d+reg(s) */
	opLDC,     /* RA     reg(r) = d ; reg(s) is ignored */
	opJLT,     /* RA     if reg(r)<0 then reg(7) = d+reg(s) */
	opJLE,     /* RA     if reg(r)<=0 then reg(7) = d+reg(s) */
	opJGT,     /* RA     if reg(r)>0 then reg(7) = d+reg(s) */
	opJGE,     /* RA     if reg(r)>=0 then reg(7) = d+reg(s) */
	opJEQ,     /* RA     if reg(r)==0 then reg(7) = d+reg(s) */
	opJNE,     /* RA     if reg(r)!=0 then reg(7) = d+reg(s) */
	opJAL,     /* RA     reg(7) = d+reg(s) */
	opRALim    /* Limit of RA opcodes */
} OPCODE;

typedef enum {
	srOKAY,
	srHALT,
	srIMEM_ERR,
	srDMEM_ERR,
	srZERODIVIDE
}STEPRESULT;

//指令定义
typedef struct {
	int iop;
	int iarg1;
	int iarg2;
	int iarg3;
} INSTRUCTION;

/******** vars ********/
int iloc = 0;
int dloc = 0;
int traceflag = FALSE;
int icountflag = FALSE;

INSTRUCTION iMem[IADDR_SIZE];//指令区
int dMem[DADDR_SIZE];//内存区
int reg[NO_REGS];//寄存器

char * opCodeTab[] = { "HALT","IN","OUT","ADD","SUB","MUL","DIV","????",
/* RR opcodes */
"LD","ST","????", /* RM opcodes */
"LDA","LDC","JLT","JLE","JGT","JGE","JEQ","JNE","JAL","????"
/* RA opcodes */
};

char * stepResultTab[] = { "OK","Halted","Instruction Memory Fault",
"Data Memory Fault","Division by 0"
};

char pgmName[100];
FILE *pgm;

char in_Line[LINESIZE];
int lineLen;
int inCol;
int num;
char word[WORDSIZE];
char ch;
int done;

/********************************************/
int opClass(int c)
{
	if (c <= opRRLim) return (opclRR);
	else if (c <= opRMLim) return (opclRM);
	else                    return (opclRA);
} /* opClass */

  /********************************************/
void writeInstruction(int loc)
{
	printf("%5d: ", loc);
	if ((loc >= 0) && (loc < IADDR_SIZE))
	{
		printf("%6s%3d,", opCodeTab[iMem[loc].iop], iMem[loc].iarg1);
		switch (opClass(iMem[loc].iop))
		{
		case opclRR: printf("%1d,%1d", iMem[loc].iarg2, iMem[loc].iarg3);
			break;
		case opclRM:
		case opclRA: printf("%3d(%1d)", iMem[loc].iarg2, iMem[loc].iarg3);
			break;
		}
		printf("\n");
	}
} /* writeInstruction */

  /********************************************/
void getCh(void)
{
	if (++inCol < lineLen)
		ch = in_Line[inCol];
	else ch = ' ';
} /* getCh */

  /*************** 取得本行中下一个不是空格的字符**********************/
int nonBlank(void)
{
	while ((inCol < lineLen)
		&& (in_Line[inCol] == ' '))
		inCol++;
	if (inCol < lineLen)
	{
		ch = in_Line[inCol];
		return TRUE;
	}
	else
	{
		ch = ' ';
		return FALSE;
	}
} /* nonBlank */

  /********************************************/
int getNum(void)
{
	int sign;
	int term;
	int temp = FALSE;
	num = 0;
	do
	{
		sign = 1;
		while (nonBlank() && ((ch == '+') || (ch == '-')))
		{
			temp = FALSE;
			if (ch == '-')  sign = -sign;
			getCh();
		}
		term = 0;
		nonBlank();
		while (isdigit(ch))
		{
			temp = TRUE;
			term = term * 10 + (ch - '0');
			getCh();
		}
		num = num + (term * sign);
	} while ((nonBlank()) && ((ch == '+') || (ch == '-')));
	return temp;
} /* getNum */

  /*************得到下一个单词*******************************/
int getWord(void)
{
	int temp = FALSE;
	int length = 0;
	if (nonBlank())
	{
		while (isalnum(ch))
		{
			if (length < WORDSIZE - 1) word[length++] = ch;
			getCh();
		}
		word[length] = '\0';
		temp = (length != 0);
	}
	return temp;
} /* getWord */

  /************跳过一个字符********************************/
int skipCh(char c)
{
	int temp = FALSE;
	if (nonBlank() && (ch == c))
	{
		getCh();
		temp = TRUE;
	}
	return temp;
} /* skipCh */

  /********************************************/
int atEOL(void)
{
	return (!nonBlank());
} /* atEOL */

  /********************************************/
int error(char * msg, int lineNo, int instNo)
{
	printf("Line %d", lineNo);
	if (instNo >= 0) printf(" (Instruction %d)", instNo);
	printf("   %s\n", msg);
	return FALSE;
} /* error */

  /****************读取指令****************************/
int readInstructions(void)
{
	OPCODE op;
	int arg1, arg2, arg3;
	int loc, regNo, lineNo;
	//寄存器清零
	for (regNo = 0; regNo < NO_REGS; regNo++)
		reg[regNo] = 0;
	dMem[0] = DADDR_SIZE - 1;
	//初始化数据区
	for (loc = 1; loc < DADDR_SIZE; loc++)
		dMem[loc] = 0;
	//初始化指令区
	for (loc = 0; loc < IADDR_SIZE; loc++)
	{
		iMem[loc].iop = opHALT;
		iMem[loc].iarg1 = 0;
		iMem[loc].iarg2 = 0;
		iMem[loc].iarg3 = 0;
	}
	lineNo = 0;
	while (!feof(pgm))
	{
		//读入一行代码
		fgets(in_Line, LINESIZE - 2, pgm);
		inCol = 0;
		lineNo++;
		lineLen = strlen(in_Line) - 1;
		if (in_Line[lineLen] == '\n') in_Line[lineLen] = '\0';
		else in_Line[++lineLen] = '\0';
		//printf("%s\n\n", in_Line);
		if ((nonBlank()) && (in_Line[inCol] != '*'))
		{
			if (!getNum())
				return error("Bad location", lineNo, -1);
			loc = num;
			if (loc > IADDR_SIZE)
				return error("Location too large", lineNo, loc);
			if (!skipCh(':'))
				return error("Missing colon", lineNo, loc);
			if (!getWord())
				return error("Missing opcode", lineNo, loc);
			op = opHALT;
			while ((op < opRALim)
				&& (strncmp(opCodeTab[op], word, 4) != 0))
				op++;
			if (strncmp(opCodeTab[op], word, 4) != 0)
				return error("Illegal opcode", lineNo, loc);
			switch (opClass(op))
			{
			case opclRR:
				/***********************************/
				if ((!getNum()) || (num < 0) || (num >= NO_REGS))
					return error("Bad first register", lineNo, loc);
				arg1 = num;
				if (!skipCh(','))
					return error("Missing comma", lineNo, loc);
				if ((!getNum()) || (num < 0) || (num >= NO_REGS))
					return error("Bad second register", lineNo, loc);
				arg2 = num;
				if (!skipCh(','))
					return error("Missing comma", lineNo, loc);
				if ((!getNum()) || (num < 0) || (num >= NO_REGS))
					return error("Bad third register", lineNo, loc);
				arg3 = num;
				break;

			case opclRM:
			case opclRA:
				/***********************************/
				if ((!getNum()) || (num < 0) || (num >= NO_REGS))
					return error("Bad first register", lineNo, loc);
				arg1 = num;
				if (!skipCh(','))
					return error("Missing comma", lineNo, loc);
				if (!getNum())
					return error("Bad displacement", lineNo, loc);
				arg2 = num;
				if (!skipCh('(') && !skipCh(','))
					return error("Missing LParen", lineNo, loc);
				if ((!getNum()) || (num < 0) || (num >= NO_REGS))
					return error("Bad second register", lineNo, loc);
				arg3 = num;
				break;
			}
			iMem[loc].iop = op;
			iMem[loc].iarg1 = arg1;
			iMem[loc].iarg2 = arg2;
			iMem[loc].iarg3 = arg3;
		}
	}
	return TRUE;
} /* readInstructions */


  /********************************************/
STEPRESULT stepTM(void)
{
	INSTRUCTION currentinstruction;
	int pc;
	int r, s, t, m;
	int ok;

	pc = reg[PC_REG];
	if ((pc < 0) || (pc > IADDR_SIZE))
		return srIMEM_ERR;
	reg[PC_REG] = pc + 1;
	currentinstruction = iMem[pc];
	switch (opClass(currentinstruction.iop))
	{
	case opclRR:
		/***********************************/
		r = currentinstruction.iarg1;
		s = currentinstruction.iarg2;
		t = currentinstruction.iarg3;
		break;

	case opclRM:
		/***********************************/
		r = currentinstruction.iarg1;
		s = currentinstruction.iarg3;
		m = currentinstruction.iarg2 + reg[s];
		if ((m < 0) || (m > DADDR_SIZE))
			return srDMEM_ERR;
		break;

	case opclRA:
		/***********************************/
		r = currentinstruction.iarg1;
		s = currentinstruction.iarg3;
		m = currentinstruction.iarg2 + reg[s];
		break;
	} /* case */

	switch (currentinstruction.iop)
	{ /* RR instructions */
	case opHALT:
		/***********************************/
		printf("HALT: %1d,%1d,%1d\n", r, s, t);
		return srHALT;
		/* break; */

	case opIN:
		/***********************************/
		do
		{
			printf("Enter value for IN instruction: ");
			fflush(stdin);
			fflush(stdout);
			gets(in_Line);
			lineLen = strlen(in_Line);
			inCol = 0;
			ok = getNum();
			if (!ok) printf("Illegal value\n");
			else reg[r] = num;
		} while (!ok);
		break;

	case opOUT:
		printf("OUT instruction prints: %d\n", reg[r]);
		break;
	case opADD:  reg[r] = reg[s] + reg[t];  break;
	case opSUB:  reg[r] = reg[s] - reg[t];  break;
	case opMUL:  reg[r] = reg[s] * reg[t];  break;

	case opDIV:
		/***********************************/
		if (reg[t] != 0) reg[r] = reg[s] / reg[t];
		else return srZERODIVIDE;
		break;

		/*************** RM instructions ********************/
	case opLD:    reg[r] = dMem[m];  break;//数据区复制给寄存器
	case opST:    dMem[m] = reg[r];  break;//寄存器复制给数据区

	/*************** RA instructions ********************/
	case opLDA:    reg[r] = m; break;
	case opLDC:    reg[r] = currentinstruction.iarg2;   break;
	case opJLT:    if (reg[r] < 0) reg[PC_REG] = m; break;
	case opJLE:    if (reg[r] <= 0) reg[PC_REG] = m; break;
	case opJGT:    if (reg[r] > 0) reg[PC_REG] = m; break;
	case opJGE:    if (reg[r] >= 0) reg[PC_REG] = m; break;
	case opJEQ:    if (reg[r] == 0) reg[PC_REG] = m; break;
	case opJNE:    if (reg[r] != 0) reg[PC_REG] = m; break;
	case opJAL:    reg[PC_REG] = m; break;

		/* end of legal instructions */
	}/* case */
	return srOKAY;
} /* stepTM */

  /**************执行指令******************************/
int doCommand(void)
{
	char cmd;
	int stepcnt = 0, i;
	int printcnt;
	int stepResult;
	int regNo, loc;
	do
	{
		printf("Enter command: ");
		fflush(stdin);
		fflush(stdout);
		gets(in_Line);
		lineLen = strlen(in_Line);
		inCol = 0;
	} while (!getWord());

	cmd = word[0];
	switch (cmd)
	{
	case 't':
		/***********************************/
		traceflag = !traceflag;
		printf("Tracing now ");
		if (traceflag) printf("on.\n"); else printf("off.\n");
		break;

	case 'h':
		/***********************************/
		printf("Commands are:\n");
		printf("   s(tep <n>      "\
			"Execute n (default 1) TM instructions\n");
		printf("   g(o            "\
			"Execute TM instructions until HALT\n");
		printf("   r(egs          "\
			"Print the contents of the registers\n");
		printf("   i(Mem <b <n>>  "\
			"Print n iMem locations starting at b\n");
		printf("   d(Mem <b <n>>  "\
			"Print n dMem locations starting at b\n");
		printf("   t(race         "\
			"Toggle instruction trace\n");
		printf("   p(rint         "\
			"Toggle print of total instructions executed"\
			" ('go' only)\n");
		printf("   c(lear         "\
			"Reset simulator for new execution of program\n");
		printf("   h(elp          "\
			"Cause this list of commands to be printed\n");
		printf("   q(uit          "\
			"Terminate the simulation\n");
		break;

	case 'p':
		/***********************************/
		icountflag = !icountflag;
		printf("Printing instruction count now ");
		if (icountflag) printf("on.\n"); else printf("off.\n");
		break;

	case 's':
		/***********************************/
		if (atEOL())  stepcnt = 1;
		else if (getNum())  stepcnt = abs(num);
		else   printf("Step count?\n");
		break;

	case 'g':   stepcnt = 1;     break;

	case 'r':
		/*************输出寄存器的内容**********************/
		for (i = 0; i < NO_REGS; i++)
		{
			printf("%1d: %4d    ", i, reg[i]);
			if ((i % 4) == 3) printf("\n");
		}
		break;

	case 'i':
		/***********输出指令区的内容************************/
		printcnt = 1;
		if (getNum())
		{
			iloc = num;
			if (getNum()) printcnt = num;
		}
		if (!atEOL())
			printf("Instruction locations?\n");
		else
		{
			while ((iloc >= 0) && (iloc < IADDR_SIZE)
				&& (printcnt > 0))
			{
				writeInstruction(iloc);
				iloc++;
				printcnt--;
			}
		}
		break;

	case 'd':
		/*******输出数据区的内容****************************/
		printcnt = 1;
		if (getNum())
		{
			dloc = num;
			if (getNum()) printcnt = num;
		}
		if (!atEOL())
			printf("Data locations?\n");
		else
		{
			while ((dloc >= 0) && (dloc < DADDR_SIZE)
				&& (printcnt > 0))
			{
				printf("%5d: %5d\n", dloc, dMem[dloc]);
				dloc++;
				printcnt--;
			}
		}
		break;

	case 'c':
		/***********设置寄存器和数据区的内容为初始值*************/
		iloc = 0;
		dloc = 0;
		stepcnt = 0;
		for (regNo = 0; regNo < NO_REGS; regNo++)
			reg[regNo] = 0;
		dMem[0] = DADDR_SIZE - 1;
		for (loc = 1; loc < DADDR_SIZE; loc++)
			dMem[loc] = 0;
		break;

	case 'q': return FALSE;  /* break; */

	default: printf("Command %c unknown.\n", cmd); break;
	}  /* case */
	stepResult = srOKAY;
	if (stepcnt > 0)
	{
		if (cmd == 'g')
		{
			stepcnt = 0;
			while (stepResult == srOKAY)
			{
				iloc = reg[PC_REG];
				if (traceflag) writeInstruction(iloc);
				stepResult = stepTM();
				stepcnt++;
			}
			if (icountflag)
				printf("Number of instructions executed = %d\n", stepcnt);
		}
		else
		{
			while ((stepcnt > 0) && (stepResult == srOKAY))
			{
				iloc = reg[PC_REG];
				if (traceflag) writeInstruction(iloc);
				stepResult = stepTM();
				stepcnt--;
			}
		}
		printf("%s\n", stepResultTab[stepResult]);
	}
	return TRUE;
}/* doCommand */


 /********************************************/
 /* E X E C U T I O N   B E G I N S   H E R E */
 /********************************************/

main(int argc, char * argv[])
{
	if (argc != 2)
	{
		printf("usage: %s <filename>\n", argv[0]);
		exit(1);
	}
	strcpy(pgmName, argv[1]);
	if (strchr(pgmName, '.') == NULL)
		strcat(pgmName, ".tm");
	char *pgmName1;
	pgmName1 = "D:\\code\\cplusplus\\tiny\\sample.tm";//"D:\\code\\Visual Studio\\visual studio 2015\\Projects\\TINY\\tm\\Debug\\sample.tm";
	printf("%s\n", pgmName1);
	pgm = fopen(pgmName1, "r");
	if (pgm == NULL)
	{
		printf("file '%s' not found\n", pgmName);
		exit(1);
	}

	/* read the program 把指令加载到指令区*/
	if (!readInstructions())
		exit(1);
	/* switch input file to terminal */
	/* reset( input ); */
	/* read-eval-print */
	printf("TM  simulation (enter h for help)...\n");
	do
		done = !doCommand();//把指令加载到指令区
	while (!done);
	printf("Simulation done.\n");
	return 0;
}
