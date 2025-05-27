void	InitDynM(void);

void*	myalloc(void);

/*	source is in Upper RAM, destination is in Lower RAM */
void	ReadMem(char* dest, char* source, unsigned char count);

/*	source is in Lower RAM, destination is in Upper RAM */
void	WriteMem(char* source, char* dest, unsigned char count);

/*	dest is in Upper RAM */
void	PutByte(char* dest, unsigned char offset, char b);

/*	source is in Upper RAM */
char	GetByte(char* source, unsigned char offset);

/*	dest is in Upper RAM */
void	PutWord(char* dest, unsigned char offset, short w);

/*	source is in Upper RAM */
short	GetWord(char* source, unsigned char offset);

bool	IsValid(char* addr);

