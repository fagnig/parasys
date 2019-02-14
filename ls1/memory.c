/* memory.c 

02346, F19, DTU

For instructions in compiling, see the header of pointers.c

Your job is to change the value of the variable 'number', to ensure that the exact same data is present in memory at
number and the char[] word. This should also give you some understanding of hex values and knowledge of the ascii table.

This exercise should help you understand that all sorts of data is just stored 
in bytes in your memory - chars are one byte, shorts two, ints 4, long longs 8.
As such, it is possible to store to shorts in an int, or 8 chars in a long long.

- e.g. can the number 90 be a short, an int, and even a char ('Z').
Best of luck!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){

	const char * word = "Hi There";

	//You are allowed to edit in the following number:
	unsigned long long number = 7310016643938150728;
	//Editing stops now

	//Consider googling endianness, as almost all machines are little-endian, 
	//and it's good to understand what this means for the way a number is stored in memory.

	//http://www.cplusplus.com/reference/cstring/memcmp/ - for information about memcmp.

	//Compares the value at 'word' and 'number'
	if (!(memcmp(&number, word, sizeof(number)))){
		printf("Succes\n");
	} else {
		printf("Not a match\n");
	}


}


	/*
	Hint: Consider using the following:

	unsigned long long num = *((unsigned long long*) word); //This prints something, but what?
	printf("0x%llx\n", num);
	*/

