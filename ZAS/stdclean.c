#include	<stdio.h>

_cleanup()
{
	uchar	i;
	register struct _iobuf *	ip;

	i = 5;
	ip = _iob;
	do {
		fclose(ip);
		ip++;
	} while(--i);
}
/*
 *	Initial setup for stdio
 */

FILE	_iob[5] =
{
	{
		(char *)0,
		0,
		(char *)0,
		_IOREAD|_IOMYBUF,
		0			/* stdin */
	},
	{
		(char *)0x7800,
		0,
		(char *)0x7800,
		_IOWRT|_IONBF,
		1			/* stdout */
	},
	{
		(char *)0x7A00,
		0,
		(char *)0x7A00,
		_IOWRT|_IONBF,
		2			/* stderr */
	},
	{
		(char *)0x7C00,
		0,
		(char *)0x7C00,
		_IONBF,
		3			/* as file */
	},
	{
		(char *)0x7E00,
		0,
		(char *)0x7E00,
		_IONBF,
		4			/* obj file */
	}
};
