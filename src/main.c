#include <stdio.h>
#include <stdarg.h>
#include <hake.tab.h>
#include <hakedef.h>

extern FILE* hkin;
extern int hklineno;

void hkerror(const char *s,...)
{
	va_list vargs;
	va_start(vargs,s);

	fprintf(stderr,"Error at line %d: ",hklineno);
	fprintf(stderr,s,vargs);
	fputs("\n",stderr);

	va_end(vargs);
}

int main(int argc,char** argv)
{
	if(argc<2)
	{
		puts("Error: Too few arguments.");
		return -1;
	}

	const char* src_filename=argv[1];

	if((hkin=fopen(src_filename,"rb"))==NULL)
	{
		printf("Error: Error opening file:%s\n",src_filename);
		return 1;
	}

	hkparse();

	fclose(hkin);

	return 0;
}
