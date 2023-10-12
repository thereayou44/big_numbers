#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int number_of_digits(long long a)
{
  if (a == 0)
    return 1 ;
  int cnt = 0 ;
  while (a)
  {
    a /= 10;
    cnt ++;
  }
  return cnt;
}
int max(int a, int b)
{
  if (a > b)
    return a ;
  else
    return b;
}
int min(int a, int b)
{
  if (a < b)
    return a ;
  else
    return b;
}
typedef struct bn_s{
  int bodysize;
  long long *body;
  int sign;
}bn;

long long base = 1000000000;
int sz = 9;

bn* bn_add(bn const *a, bn const *b);

bn* bn_sub(bn const *a, bn const *b);

int bn_cmp(bn const *a, bn const *b);

void bn_print(bn const *c);

int bn_delete(bn *t);

int bn_init_string(bn *t, const char *init_string);

bn *bn_init(bn const *orig);

int bn_copy(bn* dest, const bn* src);

int bn_init_int(bn *t , long long init_int);

void bn_print(bn const *c);

bn* bn_sub(bn const *a, bn const *b);

bn *bn_new();