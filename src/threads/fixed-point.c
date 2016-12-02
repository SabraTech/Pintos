#include "threads/fixed-point.h"

/* Convert integer to fixed point */
fixedpoint to_fixed_point (int n)
{
  return n * F;
}

/* Convert x to integer (rounding toward zero) */
int floor_to_int (fixedpoint x)
{
  return x / F;
}

/* Convert x to integer (rounding to nearest) */
int round_to_int (fixedpoint x)
{
  return x >= 0 ? (x + F / 2) / F : (x - F / 2) / F;
}

/* Add x and y */
fixedpoint add_fixed_point (fixedpoint x, fixedpoint y)
{
  return x + y;
}

/* Add x and n */
fixedpoint add_int (fixedpoint x, int n)
{
  return x + (n * F);
}

/* Subtract y from x */
fixedpoint sub_fixed_point (fixedpoint x, fixedpoint y)
{
  return x - y;
}

/* Subtract n from x */
fixedpoint sub_int (fixedpoint x, int n)
{
  return x - (n * F);
}

/* Multiply x by y */
fixedpoint mul_fixed_point (fixedpoint x, fixedpoint y)
{
  return ((int64_t) x) * y / F;
}

/* Multiply x by n */
fixedpoint mul_int (fixedpoint x, int n)
{
  return x * n;
}

/* Divide x by y */
fixedpoint div_fixed_point (fixedpoint x, fixedpoint y)
{
  return ((int64_t) x) * F / y;
}

/* Divide x by n */
fixedpoint div_int (fixedpoint x, int n)
{
  return x / n;
}
