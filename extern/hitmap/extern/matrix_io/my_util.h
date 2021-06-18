#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>

#define MIN(a,b)  (((a)<(b))? (a) : (b))
#define MAX(a,b)  (((a)>(b))? (a) : (b))

void *my_malloc (int sz);
int random_integer (int low, int high);
double random_double (double low, double high);
int str_to_mem_unit(char *str);
