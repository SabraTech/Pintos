
#define F (1 << 14)


/* x and y denote fixed_point numbers. */
/*  n is an integer. */

int to_fp(int n);
int to_int_floor(int x);
int to_int_round(int x);
int add_int(int x, int n);
int add_fp(int x, int y);
int sub_int(int x, int n);
int sub_fp(int x, int y);
int mul_int(int x, int n);
int mul_fp(int x, int y);
int div_int(int x, int n);
int div_fp(int x, int y);

/* Convert n to fixed point. */
int to_fp(int n){
  return n * F;
}

/* Convert x to integer (rounding toward zero). */
int to_int_floor(int x){
    return x/F;
}

/* Convert x to integer (rounding to nearest). */
int to_int_round(int x){
  if(x >= 0){
    return (x + F/2)/F;
  }else{
    return (x - F/2)/F;
  }
}

/* Add x and y. */
int add_fp(int x, int y){
  return x + y;
}

/* Add x and n. */
int add_int(int x, int n){
  return x + (n * F);
}

/* Subtract y from x. */
int sub_fp(int x, int y){
  return x - y;
}

/* Subtract n from x. */
int sub_int(int x, int n){
  return x - (n * F);
}

/* Multiply x by y. */
int mul_fp(int x, int y){
  return ((int64_t) x) * y/F;
}

/* Multiply x by n. */
int mul_int(int x, int n){
  return x * n;
}

/* Divide x by y. */
int div_fp(int x, int y){
  return ((int64_t)x) * F/y;
}

/* Divide x by n. */
int div_int(int x, int n){
  return x/n;
}
