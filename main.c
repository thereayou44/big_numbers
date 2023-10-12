#include "main.h"

bn *bn_new() {
  bn *r = (bn *) malloc(sizeof(bn));
  if (r == NULL) return NULL;
  r->bodysize = 1;
  r->sign = 0;
  r->body = (long long *) malloc(sizeof(long long) * r->bodysize);
  if (r->body == NULL) {
    free(r);
    return NULL;
  }
  r->body[0] = 0;
  return r;
}
int bn_init_string(bn *t, const char *init_string) {
  int i = 0;
  if (init_string == "0")
    t->sign = 0;
  else if (init_string[0] == '-') {
    i = 1;
    t->sign = -1;
  } else
    t->sign = 1;
  t->bodysize = (int) (strlen(init_string) - (t->sign == -1) + sz - 1) / sz;
  t->body = (long long *) realloc(t->body, t->bodysize * sizeof(long long));
  if (!t->body)
    return 1;
  for (int i = 0; i < t->bodysize - 1; i++) {
    char *sub_str = malloc(sz + 1);
    strncpy(sub_str,
            &init_string[strlen(init_string) - ((i + 1) * sz)],
            (strlen(init_string) - i * sz) > sz ? sz : (strlen(init_string) - i * sz));
    t->body[i] = strtol(sub_str, NULL, 10);
  }
  char *ost = malloc(sz + 1);
  long long rem = (long long) strlen(init_string) - (t->bodysize - 1) * sz;
  for (int i = 0 + (t->sign == -1); i < rem; i++)
    ost[i - (t->sign == -1)] = init_string[i];
  t->body[t->bodysize - 1] = strtol(ost, NULL, 10);

  return 0;
}
bn *bn_init(bn const *orig) {
  bn *q = bn_new();
  q->body = (long long *) malloc(sizeof(long long) * orig->bodysize);
  for (int i = 0; i < orig->bodysize; i++)
    q->body[i] = orig->body[i];
  q->bodysize = orig->bodysize;
  q->sign = orig->sign;
  return q;
}
int bn_copy(bn *dest, const bn *src) {
  if (!dest || !dest->body)
    dest = bn_new();
  dest->sign = src->sign;
  dest->bodysize = src->bodysize;
  dest->body = (long long *) (realloc(dest->body, dest->bodysize * sizeof(long long)));
  for (int i = 0; i < dest->bodysize; i++)
    dest->body[i] = src->body[i];
  return 1;
}
int bn_init_int(bn *t, long long init_int) {
  if (init_int > 0)
    t->sign = 1;
  else if (init_int == 0)
    t->sign = 0;
  else
    t->sign = -1;
  int size = 1;
  long long init_int_2 = init_int;
  while (init_int_2 > base - 1) {
    init_int_2 /= base;
    size++;
  }
  long long *a = (long long *) malloc(sizeof(long long) * size);
  int i = 0;
  while (init_int > base - 1) {
    a[i] = init_int % base;
    //printf("%lld\n", a[i]);
    init_int /= base;
    i++;
  }
  t->bodysize = size;
  for (int i = 0; i < t->bodysize - 1; i++)
    t->body[i] = a[i];
  t->body[t->bodysize - 1] = init_int;
  free(a);
  return 0;
}
void bn_print(bn const *c) {
  if (c->bodysize == 1 && c->body[0] == 0)
    printf("%d", 0);
  else {
    if (c->sign == -1)
      printf("%c", '-');
    int was_smth = 1;
    for (int i = c->bodysize - 1; i >= 0; i--) {
      {
        if (c->body[i] != 0 || (c->body[i] == 0 && was_smth == 0)) {
          int numbers_of_zeroes = sz - number_of_digits(c->body[i]);
          if (was_smth == 0) {
            for (int j = 0; j < numbers_of_zeroes; j++)
              printf("%d", 0);
          }
          printf("%lld", (c->body[i]));
          was_smth = 0;
        }
      }
    }
  }
}
int bn_cmp(bn const *a, bn const *b);
int bn_delete(bn *t);
int bn_neg(bn *t);
int bn_abs(bn *t);
int bn_sign(bn const *t);
int bn_add_to(bn *a, bn const *b);
int bn_sub_to(bn *a, bn const *b);
bn *bn_sub(bn const *a, bn const *b);
bn *bn_mul(const bn *a, const bn *b);
bn *bn_mul(const bn *a, const bn *b);

int bn_cmp(bn const *a, bn const *b) {
  if (a->sign > b->sign || a->bodysize > b->bodysize)
    return 1;
  else if (a->sign < b->sign || b->bodysize > a->bodysize)
    return -1;
  else {
    for (int i = a->bodysize - 1; i >= 0; i--)
      if (a->body[i] > b->body[i])
        return 1;
      else if (a->body[i] < b->body[i])
        return -1;
    return 0;
  }
}
int bn_delete(bn *t) {
  if (!t) {
    //printf("TROBL1");
    return 1;
  }
  free(t->body);
  free(t);
  return 1;
}
int bn_neg(bn *t) {
  t->sign *= -1;
  return 0;
}
int bn_abs(bn *t) {
  if (t->sign == 0)
    return 0;
  t->sign = 1;
  return 0;
}
int bn_sign(bn const *t) {
  return t->sign;
}
bn *bn_add(bn const *a, bn const *b) {
  bn *c = bn_new();
  if (a->sign == b->sign || a->sign == 0 || b->sign == 0) {
    long long num;
    int carry = 0;
    if (a->bodysize < b->bodysize)
      return bn_add(b, a);
    if (a->sign != 0)
      c->sign = a->sign;
    else if (b->sign != 0)
      c->sign = b->sign;
    else {
      c->sign = 0;
      return c;
    }
    c->bodysize = a->bodysize;
    c->body = (long long *) malloc(c->bodysize * sizeof(long long));
    int i = 0;
    for (; i < b->bodysize; i++) {
      num = a->body[i] + b->body[i] + carry;
      if (num >= base) {
        c->body[i] = (num - base);
        carry = 1;

      } else {
        c->body[i] = num;
        carry = 0;
      }
    }
    for (; i < a->bodysize; i++) {
      num = a->body[i] + carry;
      if (num >= base) {
        c->body[i] = num - base;
        carry = 1;
      } else {
        c->body[i] = num;
        carry = 0;
      }
    }
    if (carry) {
      c->bodysize = a->bodysize + 1;
      c->body = (long long *) realloc(c->body, c->bodysize * sizeof(long long));
      c->body[i] = carry;
    }
  } else if (a->sign == 1 && b->sign == -1) {
    bn *d;
    d = bn_init(b);
    d->sign = 1;
    c = bn_sub(a, d);
    bn_delete(d);
    return c;
  } else if (a->sign == -1 && b->sign == 1) {
    bn *d;
    d = bn_init(a);
    d->sign = 1;
    c = bn_sub(b, d);
    bn_delete(d);
    return c;
  }
  return c;
}
int bn_add_to(bn *a, bn const *b) {
  bn *c;
  c = bn_add(a, b);
  bn_copy(a, c);
  bn_delete(c);
  return 1;
}
int bn_add_to_digit(bn *a, long long b) {
  bn *c = bn_new();
  bn_init_int(c, b);
  if (b > 0)
    bn_add_to(a, c);
  else if (b < 0)
    bn_sub_to(a, c);
  else
    return 1;
  bn_delete(c);
  return 1;
}
int bn_sub_to(bn *a, bn const *b) {
  bn *c;
  c = bn_sub(a, b);
  bn_copy(a, c);
  bn_delete(c);
  return 1;
}
int bn_mul_to(bn *a, bn const *b) {
  bn *c;
  c = bn_mul(a, b);
  bn_copy(a, c);
  bn_delete(c);
  return 1;
}
int bn_mul_to_digit(bn *t, long long num, int shift) {
  if (!t || !t->body)
    return 1;
  else if (t->sign == 0 || num == 0) {
    t->bodysize = 1;
    t->body[0] = 0;
    t->sign = 0;
    return 1;
  }
  long long cell_sz = 1;
  for (int i = 0; i < sz; i++) {
    cell_sz *= 10;
  }

  long long rem = 0;
  for (int i = 0; i < t->bodysize; i++) {
    t->body[i] = t->body[i] * num + rem;
    rem = t->body[i] / cell_sz;
    t->body[i] %= cell_sz;
  }
  if (rem != 0) {
    t->bodysize++;
    t->body = (long long *) realloc(t->body, t->bodysize * sizeof(long long));
    if (!t->body)
      return 1;
    t->body[t->bodysize - 1] = rem;
  }

  if (shift != 0) {
    t->bodysize += shift;
    t->body = (long long *) realloc(t->body, t->bodysize * sizeof(long long));
    if (!t->body)
      return 1;
    for (int i = t->bodysize - 1; i >= shift; i--)
      t->body[i] = t->body[i - shift];
    for (int i = 0; i < shift; i++)
      t->body[i] = 0;
  }

  return 0;
}
void remove_leading_zeroes(bn *t) {
  for (int i = t->bodysize - 1; i > 0 && t->body[i] == 0; i--) {
    t->bodysize--;
    t->body = (long long *) (realloc(t->body, t->bodysize * sizeof(long long)));
  }
}
bn *bn_div(bn *a, bn *b) {
  if (b->sign == 0)
    return NULL;
  bn *result = bn_new();
  result->sign = (a->sign == b->sign ? 1 : -1);
  bn_abs(a);
  bn_abs(b);
  if (bn_cmp(a, b) == -1 || a->sign == 0) {
    return result;
  }
  bn *current = bn_new();
  current->bodysize = 1;
  current->body = (long long *) malloc(sizeof(long long));
  result->bodysize = a->bodysize;
  result->body = (long long *) calloc(result->bodysize, sizeof(long long));
  current->sign = 1;
  int flag = 0;
  for (long long i = (a->bodysize - 1); i >= 0; i--) {
    if (flag == 1) {
      bn_mul_to_digit(current, 1, 1);
    }
    remove_leading_zeroes(current);
    flag = 1;
    current->body[0] = a->body[i];
    if (current->sign == 0 && a->body[i] != 0) {
      current->sign = 1;
    }
    //bn_print(current);
    //printf("%lld\n", a->body[i]);
    long long x = 0, l = 0, r = base;
    while (l <= r) {
      bn *t;
      long long m = (l + r) / 2;
      t = bn_init(b);
      bn_mul_to_digit(t, m, 0);
      //printf("m-%lld\n", m);
      //bn_print(t);
      if (bn_cmp(t, current) == -1 || bn_cmp(t, current) == 0) {
        x = m;
        l = m + 1;
      } else r = m - 1;
      bn_delete(t);
    }
    result->body[i] = x;
    //printf("%lld\n", x);
    bn *mult;
    mult = bn_init(b);
    bn_mul_to_digit(mult, x, 0);
    //bn_print(mult);
    bn_sub_to(current, mult);
    bn_delete(mult);
    // bn_print(current);
  }
  remove_leading_zeroes(result);
  return result;
}

bn *bn_sub(bn const *a, bn const *b) {
  bn *c = bn_new();
  if (a->sign == -1 && b->sign == 1) {
    bn *d;
    d = bn_init(a);
    d->sign = 1;
    c = bn_add(b, d);
    c->sign = -1;
    bn_delete(d);
    return c;
  } else if (a->sign == 1 && b->sign == -1) {
    bn *d;
    d = bn_init(b);
    d->sign = 1;
    c = bn_add(a, d);
    bn_delete(d);
    return c;
  } else if (a->sign == b->sign || a->sign == 0 || b->sign == 0) {
    long long num = 0;
    int carry = 0;
    if (bn_cmp(a, b) == -1) {
      c = bn_sub(b, a);
      c->sign = a->sign * -1;
      return c;
    } else if (bn_cmp(a, b) == 0) {
      c->body[0] = 0;
      c->bodysize = 1;
      return c;
    }
    c->sign = a->sign;
    c->bodysize = max(a->bodysize, b->bodysize);
    c->body = (long long *) malloc(sizeof(long long) * c->bodysize);
    int i = 0;
    for (; i < b->bodysize; i++) {
      num = (a->body[i] - b->body[i]) + carry;
      // printf("%lld\n", num);
      if (num < 0) {
        c->body[i] = num + base;
        //   printf("%lld\n", c->body[i]);
        carry = -1;
      } else {
        c->body[i] = num;
        carry = 0;
      }
    }
    for (; i < a->bodysize; i++) {
      num = a->body[i] + carry;
      if (num < 0) {
        c->body[i] = num + base;
        carry = -1;
      } else {
        c->body[i] = num;
        carry = 0;
      }
    }
    if (carry) {
      i = a->bodysize - 1;
      while ((i > 0) && (c->body[i] == 0)) i--;
      c->bodysize = i + 1;
      c->body = (long long *) realloc(c->body, c->bodysize * sizeof(long long));
      c->body[i + 1] = carry;
    }
    return c;
  }
}
int bn_div_to(bn *a, bn *b) {
  bn *c = bn_div(a, b);
  bn_copy(a, c);
  bn_delete(c);
  return 1;
}
int bn_div_to_digit(bn *a, long long b) {
  bn *c = bn_new();
  bn_init_int(c, b);
  bn_div_to(a, c);
  bn_delete(c);
  return 1;
}
int bn_div_to_two(bn *a) {
  long long rem = 0;
  long long cell_sz = 1;
  for (int i = 0; i < sz; i++)
    cell_sz *= 10;
  for (int i = a->bodysize - 1; i >= 0; i--) {
    long long b = a->body[i];
    a->body[i] = (rem * cell_sz + a->body[i]) / 2;
    rem = b % 2;
  }
  return 0;
}
bn *bn_mul(const bn *a, const bn *b) {
  bn *c = bn_new();
  int i, j;
  long long num, carry;
  if (a->sign == b->sign)
    c->sign = 1;
  else if (a->sign == 0 || b->sign == 0) {
    c->bodysize = 1;
    c->body[0] = 0;
    c->sign = 0;
    return c;
  } else
    c->sign = -1;
  c->bodysize = a->bodysize + b->bodysize + 1;
  c->body = (long long *) malloc(sizeof(long long) * c->bodysize);
  for (i = 0; i <= a->bodysize + b->bodysize; i++) c->body[i] = 0;

  for (i = 0; i < a->bodysize; i++) {
    carry = 0;
    for (j = 0; j < b->bodysize; j++) {
      num = a->body[i] * b->body[j] + c->body[i + j] + carry;
      carry = num / base;
      c->body[i + j] = num - carry * base;
    }
    c->body[i + j] = carry;
  }
  i = a->bodysize + b->bodysize - 1;
  if (c->body[i] == 0) i--;
  c->bodysize = i + 1;
  c->body = (long long *) realloc(c->body, sizeof(long long) * c->bodysize);
  return c;
}
bn *bn_pow(bn *a, int step) {
  bn *c, *d;
  c = bn_init(a);
  d = bn_init(a);
  for (int i = 1; i < step; i++) {
    bn_mul_to(d, c);
  }
  return d;
}
bn *bn_koren(bn const *a, long long basis) {
  bn *l = bn_new();
  bn *r = bn_init(a);
  bn_init_int(l, 1);
  while (bn_cmp(l, r) == -1) {
    bn *m = bn_add(l, r);
    bn_div_to_two(m);
    remove_leading_zeroes(m);
    bn *step = bn_pow(m, basis);
    if (bn_cmp(step, a) == 1) {
      bn_copy(r, m);
      bn_delete(m);
      bn_delete(step);
    } else if (bn_cmp(step, a) == -1) {
      bn_copy(l, m);
      l->body[0] += 1;
      bn_delete(m);
      bn_delete(step);
    } else {
      bn_delete(step);
      bn_delete(r);
      return m;
    }
  }
  bn *step = bn_pow(l, basis);
  if (bn_cmp(step, a) == 1) {
    bn_delete(step);
    bn_delete(r);
    l->body[0] -= 1;
    return l;
  } else {
    bn_delete(step);
    bn_delete(r);
    return l;
  }
}

int main() {
  char ch1[100000], ch2[100000];
  bn *a = bn_new();
  bn *b = bn_new();
  scanf("%100000s", ch1);
  // scanf("/\n");
  // scanf("%100000s", ch2);
  bn *c;
  int code = bn_init_string(a, ch1);
  code = bn_init_string(b, ch2);
  c = bn_koren(a, 2);
  bn_print(c);
}
