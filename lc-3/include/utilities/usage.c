#ifndef LC3_USAGE_H
#define LC3_USAGE_H

//Show usage expected and received arguments. Show usage string. Exit with status 2.
void usage(int argc)
{
	printf("Expected at least 1 argument. Received: %d\n", argc); 
	printf("lc3 [image-file] ...\n");
	exit(2);
}

#endif //LC3_USAGE_H
