#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "constants.h"


char* join_strings(char* base, char* fname, char *delim) 
{
	size_t base_size = strlen(base);
	size_t fname_size = strlen(fname);
	size_t delim_size = strlen(delim);
	char* begin = malloc(base_size + delim_size + fname_size + 1);
	char* end = begin;
	memcpy(end, base, base_size);
	end += base_size;
	memcpy(end, delim, delim_size);
	end += delim_size;
	memcpy(end, fname, fname_size);
	end += fname_size;
	*end = '\0';

	return begin;
}