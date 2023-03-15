/******************************************************
* Adapted to 512 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include "p1.h"

#define IsHeader(p) (((char*)p) < (char*)0x4000)

extern header hashtab[];

void InitHashEntry(int index, char* sym)
{
	register header* h = (char*)&hashtab[index];

	h->first = h->last = sym;

	PutWord(sym, OFF_next, (short)h);
	PutWord(sym, OFF_prev, (short)h);
}

void AddToList(register header* hdr, char* new)
{
	char* first = hdr->first;

	if (first == (char*)hdr)
	{
		hdr->first = hdr->last = new;
		PutWord(new, OFF_next, (short)hdr);
		PutWord(new, OFF_prev, (short)hdr);
	}
	else
	{
		PutWord(first, OFF_prev, (short)new);
		PutWord(new, OFF_next, (short)first);
		PutWord(new, OFF_prev, (short)hdr);
		hdr->first = new;
	}
}

void RemoveFromList(register char* sym)
{
	char* prev;
	char* next;

	prev = (char*)GetWord(sym, OFF_prev);
	next = (char*)GetWord(sym, OFF_next);

	if (IsHeader(prev))
		((header*)prev)->first = next;
	else
		PutWord(prev, OFF_next, (short)next);

	if (IsHeader(next))
		((header*)next)->last = prev;
	else
		PutWord(next, OFF_prev, (short)prev);
}

char* GetNextSym(register header* hdr, char* crt)
{
	char* next = (char*)GetWord(crt, OFF_next);

	if ((char*)hdr == next)
		return 0;
	else
		return next;
}

char* GetFirstSym(register header* hdr)
{
	char* first = hdr->first;

	if (first == (char*)hdr)
		return 0;
	else
		return first;
}

