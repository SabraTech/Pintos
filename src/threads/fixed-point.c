#include "threads/fixed-point.h"

/* Convert integer to fixed point */
int to_fixed_point (int n)
{
  return n * F;
}

/* Convert x to integer (rounding toward zero) */
int to_int_floor (int x)
{
  return x / F;
}

/* Convert x to integer (rounding to nearest) */
int to_int_round(int x)
{
  if (x >= 0)
    return (x + F/2)/F;
  else
    return (x - F/2)/F;
}

/* Add x and y */
int add_fixed_point(int x, int y)
{
  return x + y;
}

/* Add x and n */
int add_int(int x, int n)
{
  return x + (n * F);
}

/* Subtract y from x */
int sub_fixed_point(int x, int y)
{
  return x - y;
}

/* Subtract n from x */
int sub_int(int x, int n)
{
  return x - (n * F);
}

/* Multiply x by y */
int mul_fixed_point(int x, int y)
{
  return ((int64_t) x) * y / F;
}

/* Multiply x by n */
int mul_int(int x, int n)
{
  return x * n;
}

/* Divide x by y */
int div_fixed_point(int x, int y)
{
  return ((int64_t)x) * F / y;
}

/* Divide x by n */
int div_int(int x, int n)
{
  return x / n;
}
