#ifndef SET_H_INCLUDED
#define SET_H_INCLUDED

typedef struct snode
{
	int elem;
	struct snode* next;
} snode, *symset;

symset phi, declbegsys, statbegsys, facbegsys, relset;

symset createset(int data, .../* SYM_NULL */);
void destroyset(symset s);
symset uniteset(symset s1, symset s2);
int inset(int elem, symset s);
/////////////////////////////////////add function declaration
void setinsert(symset s, int elem);


#endif // SET_H_INCLUDED
