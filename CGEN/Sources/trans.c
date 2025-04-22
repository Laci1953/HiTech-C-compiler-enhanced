/* Max line width = 132 */

#include <sys.h>
#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>

FILE* in;
FILE* out;
unsigned int number;
unsigned int low, high;
unsigned int nibble1, nibble2;

int main(int argc, char*argv[])
{
  if (argc != 3)
  {
    printf("Invalid parameters!\r\nUsage is: trans out_file in_file\r\n");
    exit(0);
  }

  if (!(in=fopen(argv[2], "r")))
  {
    printf("Cannot open input file!\r\n");
    exit(0);
  }

  if (!(out=fopen(argv[1], "w")))
  {
    printf("Cannot open output file!\r\n");
    exit(0);
  }

  do 
  {
					//read 1 or 2 numbers, output 2 or 3 bytes

    fscanf(in, "%d", &number);		//number 1 in
    //printf("number=%d(%04x)\n", number, number);

    if (number == 9999)			//end-of-numbers?
    {
      fprintf(out, "0\n");
      break;				//quit
    }

    low = number & 0xFF; 
    high = (number & 0xFF00) >> 8; 

    fprintf(out, "%d, ", low);		//char 1 out (low byte of number 1)
    //printf("%04x ", low);

    nibble1 = high & 0xF; 		//low nibble from high byte of number 1

    fscanf(in, "%d", &number);		//number 2 in
    //printf("number=%d(%04x)\n", number, number);

    if (number == 9999)			//end-of-numbers?
    {
      fprintf(out, "%d\n", nibble1);		//char 2 out (low nibble from high byte of number 1, void)
      //printf("%04x ", nibble1);
      break;					//quit
    }

    low = number & 0xFF; 
    high = (number & 0xFF00) >> 8; 

    nibble2 = (low & 0xF) << 4;		//low nibble from low byte of number 2)

    fprintf(out, "%d, ", nibble1 | nibble2);	//char 2 out (low nibble of high byte of number 1,
						//            low nibble of low byte of number 2)
    //printf("%04x ", nibble1 | nibble2);

    nibble1 = (low & 0xF0) >> 4;	//high nibble of low byte of number 2
    nibble2 = (high & 0xF) << 4;	//low nibble of high byte of number 2

    fprintf(out, "%d, \n", nibble1 | nibble2);	//char 3 out (high nibble of low byte of number 2,
						//	      low nibble of high byte of number 2)
    //printf("%04x ", nibble1 | nibble2);
  } while (1 == 1);

  fclose(in);
  fclose(out);

  exit(1);
}
