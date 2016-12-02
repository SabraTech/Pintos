 #include <stdint.h>

#define F (1 << 14)

typedef int fixedpoint;

fixedpoint to_fixed_point (int n);

int floor_to_int (fixedpoint x);

int round_to_int (fixedpoint x);

fixedpoint add_int (fixedpoint x, int n);

fixedpoint add_fixed_point (fixedpoint x, fixedpoint y);

fixedpoint sub_int (fixedpoint x, int n);

fixedpoint sub_fixed_point (fixedpoint x, fixedpoint y);

fixedpoint mul_int (fixedpoint x, int n);

fixedpoint mul_fixed_point (fixedpoint x, fixedpoint y);

fixedpoint div_int (fixedpoint x, int n);

fixedpoint div_fixed_point (fixedpoint x, fixedpoint y);
