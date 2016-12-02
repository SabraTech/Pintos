 #include <stdint.h>

#define F (1 << 14)

int to_fixed_point(int n);

int to_int_floor(int x);

int to_int_round(int x);

int add_int(int x, int n);

int add_fixed_point(int x, int y);

int sub_int(int x, int n);

int sub_fixed_point(int x, int y);

int mul_int(int x, int n);

int mul_fixed_point(int x, int y);

int div_int(int x, int n);

int div_fixed_point(int x, int y);
