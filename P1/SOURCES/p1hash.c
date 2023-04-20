/******************************************************
* Adapted to 128 KB RAM systems by Ladislau Szilagyi  *
*		ladislau_szilagyi@euroqst.ro	      *
*	   December 2022 - March 2023		      *
******************************************************/

#include "p1.h"

void InitHash(void)
{
	int n;
#ifdef M2X64K
	register char* h;
#else
	register header* h;
#endif

	for (n = 0; n < HASHTABSIZE; n++)
	{
#ifdef M2X64K
		h = Get_hashtab(n);
		PutWord(h, OFF_first, (short)h);
		PutWord(h, OFF_last, (short)h);
#else
		h = &hashtab[n];
		h->first = h->last = (char*)h;
#endif 
	}

#ifdef M2X64K
	h = Get_SymList();
	PutWord(h, OFF_first, (short)h);
	PutWord(h, OFF_last, (short)h); 
#else
	SymList.first = SymList.last = (char*)&SymList;
#endif
}

void InitHashEntry(int index, char* sym)
{
#ifdef M2X64K
	register char* h = Get_hashtab(index);
#else
	register header* h = &hashtab[index];
#endif

#ifdef M2X64K
	PutWord(h, OFF_first, (short)sym);
	PutWord(h, OFF_last, (short)sym); 
#else
	h->first = h->last = sym;
#endif

	PutWord(sym, OFF_next, (short)h);
	PutWord(sym, OFF_prev, (short)h);
}

void AddToList(register char* hdr, char* new)
{
#ifdef M2X64K
	char* first = (char*)GetWord(hdr, OFF_first);
#else
	char* first = ((header*)hdr)->first;
#endif

	if (first == hdr)
	{
#ifdef M2X64K
		PutWord(hdr, OFF_first, (short)new);
		PutWord(hdr, OFF_last, (short)new);
#else
		((header*)hdr)->first = ((header*)hdr)->last = new;
#endif
		PutWord(new, OFF_next, (short)hdr);
		PutWord(new, OFF_prev, (short)hdr);
	}
	else
	{
		PutWord(first, OFF_prev, (short)new);
		PutWord(new, OFF_next, (short)first);
		PutWord(new, OFF_prev, (short)hdr);
#ifdef M2X64K
		PutWord(hdr, OFF_first, (short)new);
#else
		((header*)hdr)->first = new;
#endif
	}
}

void RemoveFromList(register char* sym)
{
	char* prev;
	char* next;

	prev = (char*)GetWord(sym, OFF_prev);
	next = (char*)GetWord(sym, OFF_next);

	if (IsHeader(prev))
#ifdef M2X64K
		PutWord(prev, OFF_first, (short)next);
#else
		((header*)prev)->first = next;
#endif
	else
		PutWord(prev, OFF_next, (short)next);

	if (IsHeader(next))
#ifdef M2X64K
		PutWord(next, OFF_last, (short)prev);
#else
		((header*)next)->last = prev;
#endif
	else
		PutWord(next, OFF_prev, (short)prev);
}

char* GetNextSym(char* hdr, register char* crt)
{
	char* next = (char*)GetWord(crt, OFF_next);

	if (hdr == next)
		return 0;
	else
		return next;
}

char* GetFirstSym(register char* hdr)
{
#ifdef M2X64K
	char* first = (char*)GetWord(hdr, OFF_first);
#else
	char* first = ((header*)hdr)->first;
#endif

	if (first == hdr)
		return 0;
	else
		return first;
}

