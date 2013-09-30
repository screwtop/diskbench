#include <stdio.h>
#include <sys/types.h>	// For off_t

int main()
{
//	printf("sizeof(byte) = %li\n", sizeof(byte));
	printf("sizeof(char) = %li\n", sizeof(char));
	printf("sizeof(short) = %li\n", sizeof(short));
	printf("sizeof(int) = %li\n", sizeof(int));
	printf("sizeof(long) = %li\n", sizeof(long));
	printf("sizeof(long long) = %li\n", sizeof(long long));
	printf("sizeof(size_t) = %li\n", sizeof(size_t));
	printf("sizeof(off_t) = %li\n", sizeof(off_t));
	return 0;
}
