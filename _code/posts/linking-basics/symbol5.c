#include <stdio.h>

char main;

void p(){
	printf("0x%x\n",*(&main+1));
	printf("0x%x\n",main);
}
