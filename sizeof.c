#include <stdio.h>
#include <sys/types.h>	// For off_t
#include <stdint.h>	// for intmax_t

// For determining whether signed (https://stackoverflow.com/questions/1412197/how-do-i-tell-if-a-c-integer-variable-is-signed)
#define ISVARSIGNED(V) ((V)-1<0 || -(V)-1<0)
#define SIGNEDNESS(V) (ISVARSIGNED(V) ? "signed" : "unsigned")

int main()
{
//	byte b=0;
	char c=0;
	short s=0;
	int i=0;
	long l=0;
	long long ll=0;
	intmax_t imt=0;
	size_t st=0;
	off_t ot=0;

//	printf("sizeof(byte) = %li, %s\n", sizeof(byte), SIGNEDNESS(b));
	printf("sizeof(char) = %li, %s\n", sizeof(char), SIGNEDNESS(c));
	printf("sizeof(short) = %li, %s\n", sizeof(short), SIGNEDNESS(s));
	printf("sizeof(int) = %li, %s\n", sizeof(int), SIGNEDNESS(i));
	printf("sizeof(long) = %li, %s\n", sizeof(long), SIGNEDNESS(l));
	printf("sizeof(long long) = %li, %s\n", sizeof(long long), SIGNEDNESS(ll));
	printf("sizeof(intmax_t) = %li, %s\n", sizeof(intmax_t), SIGNEDNESS(imt));
	printf("sizeof(size_t) = %li, %s\n", sizeof(size_t), SIGNEDNESS(st));
	printf("sizeof(off_t) = %li, %s\n", sizeof(off_t), SIGNEDNESS(ot));

	return 0;
}
