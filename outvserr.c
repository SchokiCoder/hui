#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int num = atoi(argv[1]);
	
	if (num == 0)
		fprintf(stdout, "We print to stdout and return %i\n", num);
	else
		fprintf(stderr, "We print to stderr and return %i\n", num);
	
	return num;
}

