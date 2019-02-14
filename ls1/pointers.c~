/* pointers.c 

02346, F19, DTU

In this exercise, you need to figure out which one of the two swap-functions to use, and implement said function
There's two prototypes given, and two empty function-calls.
Apart from that, you need to implement a function, that figures out the length of a string, and prints this.

The file is compiled by running gcc pointers.c -o pointers on linux, which is running on the HPC machines at DTU.

The program is called by running e.g. "./pointers string", where string is the string we'll find the length of.
If you don't wish to print the length of the string, ./pointers will run the program without arguments, and the
string printing implementation will not be tested.
*/

#include <stdio.h>

void swap0(int a, int b);
void swap1(int * a, int * b);
int getLengthOfString(char * str);

int main(int argc, char* args[]){
	int a = 5;
	int b = 7;
	
	//Do Swap by calling the function you implemented. You only need one of them, and only one of them can be made to work.
	//swapcall(a,b);
	swap1(&a,&b);

	if (a == 7 && b == 5){
		printf("Succes swapping!\n");
	}

	if (argc == 2){
		//Arrays are 0-based, however, the 0th argument in args is the name of the program.
		printf("Length of input string: %d\n", getLengthOfString(args[1]));
	}
	return 0;
}

/*
Implement me?
*/
void swap0(int a, int b){

}

/*
Implement me?
*/
void swap1(int * a, int * b){
	int tmp;
	tmp = *a;	
	*a = *b;
	*b = tmp;
}



/*
Implement me!
*/
int getLengthOfString(char * str){
	int i = 0;

	while(str[i] != '\0'){
		i++;
	}
	
	return i;
}
//Hint: How does c-style strings end? And what sort of data structure are they, as it's just a char *?


