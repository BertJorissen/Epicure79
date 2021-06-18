#define MY_UTIL_MAIN
#include "my_util.h"

void *my_malloc (int sz)
{
  void *ptr = malloc(sz);

  if (!ptr){
    printf ("Error: can't allocate %d bytes\n", sz);
    exit (0);
  }
#ifdef DEBUG
  printf("allocating %d bytes to %p\n", sz, ptr);
#endif

  return ptr;
}

/* generates random integer in [low, high) */
int random_integer (int low, int high)
{
  int r;
  r = ((high-low)*drand48()) + low;
  if (r==high) r--;

  return r;
}

/* generates random double in [low, high) */
double random_double (double low, double high)
{
    return  ((high-low)*drand48()) + low;
}
/*
MT-LEVEL
     Safe

DESCRIPTION
     This family of  functions  generates  pseudo-random  numbers
     using  the  well-known linear congruential algorithm and 48-
     bit integer arithmetic.

     Functions  drand48()  and  erand48()   return   non-negative
     double-precision floating-point values uniformly distributed
     over the interval [0.0, 1.0).
*/


int str_to_mem_unit(char *str){
  int v = 0;

  while (*str >= '0' && *str <='9'){
    v = v*10 + *str - '0'; str++;
  }

  if (*str == 'm' || *str == 'M'){
    v *= 1024*1024;
  }
  else if (*str == 'k' || *str == 'K'){
    v *= 1024;
  }

  return v;
}
