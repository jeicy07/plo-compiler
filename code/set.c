/************  SET.c  *************/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "set.h"

symset uniteset(symset s1, symset s2)
{
	symset s;
	snode* p;

	s = p = (snode*) malloc(sizeof(snode));
	while (s1 && s2)
	{
		p->next = (snode*) malloc(sizeof(snode));
		p = p->next;
		if (s1->elem < s2->elem)
		{
			p->elem = s1->elem;
			s1 = s1->next;
		}
		else
		{
			p->elem = s2->elem;
			s2 = s2->next;
		}
	}

	while (s1)
	{
		p->next = (snode*) malloc(sizeof(snode));
		p = p->next;
		p->elem = s1->elem;
		s1 = s1->next;

	}

	while (s2)
	{
		p->next = (snode*) malloc(sizeof(snode));
		p = p->next;
		p->elem = s2->elem;
		s2 = s2->next;
	}

	p->next = NULL;

	return s;
} // uniteset

void setinsert(symset s, int elem)
{
///////////////////////////////////////////////////
	snode* tmp = s;
	while(tmp -> next != NULL)
	{
		if(tmp -> elem <= elem && tmp -> next -> elem > elem)
		{
			snode* newNode = (snode*)malloc(sizeof(snode));
			newNode -> elem = elem;
			newNode -> next = tmp -> next;
			tmp -> next = newNode;
			return;
		}
		else
			tmp = tmp -> next;
	}
	snode* newNode = (snode*)malloc(sizeof(snode));
	newNode -> elem = elem;
	newNode -> next = NULL;
	tmp -> next = newNode;
	return;
///////////////////////////////////////////////
} // setinsert

symset createset(int elem, .../* SYM_NULL */)
{
	va_list list;
	symset s;

	s = (snode*) malloc(sizeof(snode));
	s->next = NULL;

	va_start(list, elem);
	while (elem)
	{
		setinsert(s, elem);
		elem = va_arg(list, int);
	}
	va_end(list);
	return s;
} // createset

void destroyset(symset s)
{
	snode* p;

	while (s)
	{
		p = s;
		s = s->next;
		free(p);
	}
} // destroyset

int inset(int elem, symset s)
{
///////////////////////////////////////add function
	snode* tmp = s;
	while(tmp != NULL && tmp -> elem != elem)
	{
		tmp = tmp -> next;
	}
	if(tmp == NULL)          //can't find the element
		return 0;
	else
		return 1;            //find the element
} // inset

// EOF set.c
