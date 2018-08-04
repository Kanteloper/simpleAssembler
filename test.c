#include <stdio.h>
#include <stdlib.h>

int main()
{
	char buff[4] = "0x4";
	printf("%ld\n", strtol(buff, NULL, 16));
	return 0;
}
