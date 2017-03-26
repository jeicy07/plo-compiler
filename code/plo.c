/************  PL0.c  *************/

// pl0 compiler source code

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "set.h"
#include "plo.h"

//////////////////////////////////////////////////////////////////////
// print error message.
void error(n)
{
	int i;

	printf("      ");
	for (i = 1; i <= cc - 1; i++)
		printf(" ");
	printf("^\n");
	printf("Line %d:Error %3d: %s\n",cx-1, n, err_msg[n]);
	err++;
} // error/Volumes/JEICY/±àÒëÔ­Àí/Source Code/pl0.c

//////////////////////////////////////////////////////////////////////
void getch(void)
{
	if (cc == ll)
	{
		if (feof(infile))
		{
			printf("\nPROGRAM INCOMPLETE\n");
			exit(1);
		}
////////////////////////////////////////////////////////print the error first
		if(final_err)
        {
            cx--;
            error(5);
            final_err = 0;
            cx++;
        }
        ll = cc = 0;
		printf("%5d  ", cx);
		while (!feof(infile) && (ch = getc(infile))!='\n')
		{
			printf("%c", ch);
			line[++ll] = ch;
		} // while
		printf("\n");
		line[++ll] = ' ';
		cx++;
    /////////////////////////////////////////////////////////////////
	}
	ch = line[++cc];
} // getch

//////////////////////////////////////////////////////////////////////
// gets a symbol from input stream.
void getsym(void)
{
	int i, k;
	char a[MAXIDLEN + 1];

	while (ch == ' ' || ch=='\t')
		getch();

	if (isalpha(ch))
	{ // symbol is a reserved word or an identifier.
		k = 0;
		do
		{
			if (k < MAXIDLEN)
				a[k++] = ch;
			getch();
		}
		while (isalpha(ch) || isdigit(ch));
		a[k] = 0;
		strcpy(id, a);
		word[0] = id;
		i = NRW;
		while (strcmp(id, word[i--]));
		if (++i)
		{
			sym = wsym[i]; // symbol is a reserved word
		}

		else
		{
			sym = SYM_IDENTIFIER;   // symbol is an identifier
		}
	}
	else if (isdigit(ch))
	{ // symbol is a number.
		k = num = 0;
		sym = SYM_NUMBER;
		do
		{
			num = num * 10 + ch - '0';
			k++;
			getch();
		}
		while (isdigit(ch));
		if (k > MAXNUMLEN)
			error(25);     // The number is too great.
	}
	else if (ch == ':')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_BECOMES; // :=
			getch();
		}
		else
		{
			sym = SYM_NULL;       // illegal?
		}
	}
	else if (ch == '>')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_GEQ;     // >=
			getch();
		}
		else
		{
			sym = SYM_GTR;     // >
		}
	}
	else if (ch == '<')
	{
		getch();
		if (ch == '=')
		{
			sym = SYM_LEQ;     // <=
			getch();
		}
		else if (ch == '>')
		{
			sym = SYM_NEQ;     // <>
			getch();
		}
		else
		{
			sym = SYM_LES;     // <
		}
	}
	else
	{ // other tokens
		i = NSYM;
		csym[0] = ch;
		while (csym[i--] != ch);
		if (++i)
		{
		      //deal with annotation
			if(ch == '(')
            {
                getch();
                if(ch == '*')           //start an annotation
                {
                        int flag = 1;   //0 when '*)'
                        while(flag)
                        {
                            getch();
                            if(ch == '*')
                            {
                                getch();
                                if(ch == ')') //the annotation ends
                                {
                                    flag = 0;
                                    getch();
                                }
                            }
                        }
                }
                else                       //just a token '('
                {
                    getch();
                    sym = ssym[i];
                }
            }
            else
            {
                sym = ssym[i];
                if (sym != SYM_PERIOD)
                {
                    getch();
                }
            }
		}
		else
		{
			printf("Fatal Error: Unknown character.\n");
			exit(1);
		}
	}
} // getsym


//////////////////////////////////////////////////////////////////////
//tests if error occurs and skips all symbols that do not belongs to s1 or s2.
void test(symset s1, symset s2, int n)
{
	symset s;

	if (! inset(sym, s1))
	{
		error(n);
		s = uniteset(s1, s2);
		while(! inset(sym, s))
			getsym();
		destroyset(s);
	}
} // test


//////////////////////////////////////////////////////////////////////
void constdeclaration()
{
    if(sym != SYM_IDENTIFIER)
	{
		error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
	}
} // constdeclaration

//////////////////////////////////////////////////////////////////////
void vardeclaration(void)
{
	if(sym != SYM_IDENTIFIER)
	{
		error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
	}
} // vardeclaration

//////////////////////////////////////////////////////////////////////
void factor(symset fsys)
{
    void expression();
	symset set;

	test(facbegsys, fsys, 24); // The symbol can not be as the beginning of an expression.

	while (inset(sym, facbegsys))
	{
		if (sym == SYM_IDENTIFIER)
		{
			getsym();
		}
		else if (sym == SYM_NUMBER)
		{
			if (num > MAXADDRESS)
			{
				error(25); // The number is too great.
				num = 0;
			}
			getsym();
		}
		else if (sym == SYM_LPAREN)
		{
			getsym();
			set = uniteset(createset(SYM_RPAREN, SYM_NULL), fsys);
			expression(set);
			destroyset(set);
			if (sym == SYM_RPAREN)
			{
				getsym();
			}
			else
			{
				error(22); // Missing ')'.
			}
		}
		test(fsys, createset(SYM_LPAREN, SYM_NULL), 23);
	} // while
} // factor

//////////////////////////////////////////////////////////////////////
void term(symset fsys)
{
	int mulop;
	symset set;

	set = uniteset(fsys, createset(SYM_TIMES, SYM_SLASH, SYM_NULL));
	factor(set);
	while (sym == SYM_TIMES || sym == SYM_SLASH)
	{
		mulop = sym;
		getsym();
		factor(set);
	} // while
	destroyset(set);
} // term

//////////////////////////////////////////////////////////////////////
void expression(symset fsys)
{
	int addop;
	symset set;

	set = uniteset(fsys, createset(SYM_PLUS, SYM_MINUS, SYM_NULL));
	if (sym == SYM_PLUS || sym == SYM_MINUS)
	{
		addop = sym;
		getsym();
		term(set);
	}
	else
	{
		term(set);
	}

	while (sym == SYM_PLUS || sym == SYM_MINUS)
	{
		addop = sym;
		getsym();
		term(set);
	} // while

	destroyset(set);
} // expression

//////////////////////////////////////////////////////////////////////
void condition(symset fsys)
{
	int relop;
	symset set;

	if (sym == SYM_ODD)
	{
		getsym();
		expression(fsys);
	}
	else
	{
		set = uniteset(relset, fsys);
		expression(set);
		destroyset(set);
		if (! inset(sym, relset))
		{
			error(20);
		}
		else
		{
			relop = sym;
			getsym();
			expression(fsys);
			//switch (relop)
			//{
			//} // switch
		} // else
	} // else
} // condition

//////////////////////////////////////////////////////////////////////
void statement(symset fsys)
{
	symset set1, set;

	if (sym == SYM_IDENTIFIER)
	{ // variable assignment
		getsym();
		if (sym == SYM_BECOMES)
		{
			getsym();
		}
		else
		{
			error(13); // ':=' expected.
		}
		expression(fsys);
	}
	else if (sym == SYM_CALL)
	{ // procedure call
		getsym();
		if (sym != SYM_IDENTIFIER)
		{
			error(14); // There must be an identifier to follow the 'call'.
		}
		else
		{
			getsym();
		}
	}
	else if (sym == SYM_IF)
	{ // if statement
		getsym();
		set1 = createset(SYM_THEN, SYM_DO, SYM_NULL);
		set = uniteset(set1, fsys);
		condition(set);
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_THEN)
		{
			getsym();
		}
		else
		{
			error(16); // 'then' expected.
		}
		statement(fsys);
	}
	else if (sym == SYM_BEGIN)
	{ // block
		getsym();
		set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
		set = uniteset(set1, fsys);
		statement(set);
		while (sym == SYM_SEMICOLON || inset(sym, statbegsys))
		{
			if (sym == SYM_SEMICOLON)
			{
				getsym();
			}
			else
			{///////////////////////////////////////////////the error line should -1
			    cx --;
				error(10);
				cx ++;
			/////////////////////////////////////////////
			}
			statement(set);
		} // while
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_END)
		{
			getsym();
		}
		else
		{
			error(17); // ';' or 'end' expected.
		}
	}
	else if (sym == SYM_WHILE)
	{ // while statement
		getsym();
		set1 = createset(SYM_DO, SYM_NULL);
		set = uniteset(set1, fsys);
		condition(set);
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_DO)
		{
			getsym();
		}
		else
		{
			error(18); // 'do' expected.
		}
		statement(fsys);
	}
	test(fsys, phi, 19);
} // statement

//////////////////////////////////////////////////////////////////////
void block(symset fsys)
{
	symset set1, set;

	if (level > MAXLEVEL)
	{
		error(32); // There are too many levels.
	}
	do{

		if (sym == SYM_CONST)
		{ // constant declarations
////////////////////////////////////////////////////////////////////////////////
			getsym();
			constdeclaration();
            set1 = createset(SYM_IDENTIFIER,SYM_NUMBER,SYM_EQU,SYM_NULL);
            while(inset(sym,set1))
            {
                if (sym == SYM_IDENTIFIER)
                    getsym();
                else
                    error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
                if (sym == SYM_EQU)
                    getsym();
                else if(sym == SYM_BECOMES)
                {
                    error(1);  //"Found ':=' when expecting '='."
                    getsym();
                }

                else
                {
                     error(26);            //"'=' expected."
                     getsym();
                }

                if (sym == SYM_NUMBER)
                    getsym();
                else
                    error(2);   //"There must be a number to follow '='."
                if (sym == SYM_SEMICOLON)
                {
                    getsym();
                    break;
                }

                else if(sym == SYM_COMMA)
                    getsym();
                else
                    final_err=1;    //"Missing ',' or ';'."
            }//while
        }// if


/////////////////////////////////////////////////////////////////////////////


		if (sym == SYM_VAR)
		{ // variable declarations
////////////////////////////////////////////////////////////////////////////////
			getsym();
			vardeclaration();
            set1 = createset(SYM_IDENTIFIER,SYM_NULL);
            while(inset(sym,set1))
            {
                if (sym == SYM_IDENTIFIER)
                    getsym();
                else
                    error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
                if (sym == SYM_SEMICOLON)
                {
                    getsym();
                    break;
                }
                else if(sym == SYM_COMMA)
                    getsym();
                else
                    final_err = 1;    //"Missing ',' or ';'."
            }//while

        }// if
/////////////////////////////////////////////////////////////////////////////


		while (sym == SYM_PROCEDURE)
		{ // procedure declarations
			getsym();
			if (sym == SYM_IDENTIFIER)
			{
				getsym();
			}
			else
			{
				error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
			}


			if (sym == SYM_SEMICOLON)
			{
				getsym();
			}
			else
			{
			    cx--;
				error(5); // Missing ',' or ';'.
				cx++;
			}

			set1 = createset(SYM_SEMICOLON, SYM_NULL);
			set = uniteset(set1, fsys);
			block(set);
			destroyset(set1);
			destroyset(set);

			if (sym == SYM_SEMICOLON)
			{
				getsym();
				set1 = createset(SYM_IDENTIFIER, SYM_PROCEDURE, SYM_NULL);
				set = uniteset(statbegsys, set1);
				test(set, fsys, 6);
				destroyset(set1);
				destroyset(set);
			}
			else
			{
				error(5); // Missing ',' or ';'.
			}
		} // while
		set1 = createset(SYM_IDENTIFIER, SYM_NULL);
		set = uniteset(statbegsys, set1);
		test(set, declbegsys, 7);
		destroyset(set1);
		destroyset(set);
	}
	while (inset(sym, declbegsys));

	set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
	set = uniteset(set1, fsys);
	statement(set);
	destroyset(set1);
	destroyset(set);
	test(fsys, phi, 8); // test for error: Follow the statement is an incorrect symbol.
} // block


//////////////////////////////////////////////////////////////////////
void main ()
{
	char s[80];
	symset set, set1, set2;

	printf("Please input source file name: "); // get file name to be compiled
	scanf("%s", s);
	if ((infile = fopen(s, "r")) == NULL)
	{
		printf("File %s can't be opened.\n", s);
		exit(1);
	}

	phi = createset(SYM_NULL);
	relset = createset(SYM_EQU, SYM_NEQ, SYM_LES, SYM_LEQ, SYM_GTR, SYM_GEQ, SYM_NULL);

	// create begin symbol sets
	declbegsys = createset(SYM_CONST, SYM_VAR, SYM_PROCEDURE, SYM_NULL);
	statbegsys = createset(SYM_BEGIN, SYM_CALL, SYM_IF, SYM_WHILE, SYM_NULL);
	facbegsys = createset(SYM_IDENTIFIER, SYM_NUMBER, SYM_LPAREN, SYM_NULL);

	err = cc = cx = ll = 0; // initialize global variables
	ch = ' ';
	kk = MAXIDLEN;

	getsym();

	set1 = createset(SYM_PERIOD, SYM_NULL);
	set2 = uniteset(declbegsys, statbegsys);
	set = uniteset(set1, set2);
	block(set);
	destroyset(set1);
	destroyset(set2);
	destroyset(set);
	destroyset(phi);
	destroyset(relset);
	destroyset(declbegsys);
	destroyset(statbegsys);
	destroyset(facbegsys);

	if (sym != SYM_PERIOD)
		error(9); // '.' expected.

	if (err == 0);
	else
		printf("There are %d error(s) in PL/0 program.\n", err);
} // main    END OF PL0.c
