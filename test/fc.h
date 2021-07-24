#pragma once
#include <cstdlib>
#include <cstdio>
#include <cstring>

namespace ds
{
	inline int fc(const char *f1,const char *f2)
	{
		FILE *file1 = fopen(f1,"r"), *file2 = fopen(f2, "r");
		fseek(file1, 0, SEEK_END), fseek(file2, 0, SEEK_END);
		int len1 = ftell(file1) + 100, len2 = ftell(file2) + 100;
		fseek(file1, 0, SEEK_SET), fseek(file2, 0, SEEK_SET);
		char *cont1 = (char *)calloc(len1, 1), *cont2 = (char *)calloc(len2, 1);
		fread(cont1, 1, len1, file1), fread(cont2, 1, len2, file2);
		int ret = strcmp(cont1, cont2);
		free(cont1), free(cont2);
		fclose(file1), fclose(file2);
		return ret;
	}
}
