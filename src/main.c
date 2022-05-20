#include <stdio.h>
#include <stdarg.h>
#include <slake.tab.h>
#include <slakedef.h>

extern FILE* slakein;
extern int slakelineno;

void slakeerror(const char *s,...)
{
	va_list vargs;
	va_start(vargs,s);

	fprintf(stderr,"Error at line %d: ",slakelineno);
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

	if((slakein=fopen(src_filename,"rb"))==NULL)
	{
		printf("Error: Error opening file:%s\n",src_filename);
		return 1;
	}

	slakeparse();

	fclose(slakein);

	return 0;
}
