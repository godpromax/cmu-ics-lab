/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 朱梓豪 202008010418 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* 
 * bitAnd - x&y using only ~ and | 
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
 //用按位取反和按位或表示按位与，德摩律 
int bitAnd(int x, int y) {
	return ~((~x)|(~y));
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
 //移位运算是按位移的，一个字节是八位，所以先给n右移3相当于+8，每次取8个位，最后0xff换成二进制就是1111 1111,相当于把字节保存下来并且前三个字节为0 
int getByte(int x, int n) {
	int result=(x>>(n<<3));
	return result & 0xff;
}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
 //计算机中的int型的右移是默认为算术右移，前面会补满首位数字，所以需要右移后把前面几位数字变成0从而达到逻辑右移的运算 
int logicalShift(int x, int n) {
    int result=x>>n;
    int a=(1<<31)>>(n-1);
    return result & (~a);
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
    int count=0x00;
    for(int i=0x20;i>0;i--){
    	if(x & 0x01)
    	count=count+1;
    	x=x>>1;
	}
    return count;
}
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
 //考虑到对于每一个非0的int型数据x，将其和它的相反数-x进行或运算后，最高位为1，而如果x为0，进行同样的操作后，最高位仍为0。
 //因此可以将以上述运算得到的结果右移31位，最后将这个数加1，就能得到与!运算相同的结果。
int bang(int x) {
    return ((x | ((~x) + 1)) >> 31) + 1;
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
 //confused
int tmin(void) {
    return 1 << 31;
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
	int sn=32-n;
    int xx=x<<sn>>sn;
    return !(xx^x);//和原来相同返回1，表示能用n位表示
}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
 //正数时直接右移即可，负数右移和除法不一样，需要加上一个偏移量 
int divpwr2(int x, int n) {
    int sign=x>>31;//用来判断是否需要偏移量 
    int offset=sign&((1<<n)+(~0));
    return (x+offset)>>n;
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
    return ~x+1;
}
/* 
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
 //先判断有没有值，再判断符号位 
int isPositive(int x) {
    int sign=x>>31;
    int value=!x;
    return !(sign | value);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    int signx=(x>>31) & 0x1;
	int signy=(y>>31) & 0x1;
    int sign=x ^ y;
    //先判断x和y同号或者异号
	if(sign & signx ) return 1;//异号 
    if(~sign){
    	int result=((y+~x+1)>>31) & 0x1;//y-x的符号位
		if(result) return 0;
		else return 1; 
	}
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
 //该函数实际上就是要找到最接近一个数n使得2n最接近x，且满足2n <= x。因此可以先将int型数据x右移16位，并进行两次取反操作，如果得到的值为1
 //则说明x的高16位中存在至少一个1，那么result应加上16；如果得到的值为0，则说明高16位中不存在1。然后再将x右移(result+8)位，同样进行两次取反操作
 //如果得到的值为1,则说明在(result+8)和(result+15)这8位中至少有一个1，那么result应加上8；如果得到的值为0，则说明这8位中不存在1。
 //依次类推，继续将x右移(result+4)，(result+2)，(result+1)位，并进行同样的操作即可得到最终结果。
int ilog2(int x) {
    int result = 0;
	result = (!!(x >> 16)) << 4;
	result = result + ((!!(x >> (result + 8))) << 3);
	result = result + ((!!(x >> (result + 4))) << 2);
	result = result + ((!!(x >> (result + 2))) << 1);
	result = result + (!!(x >> (result + 1)));
	return result;
}
/* 
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
    unsigned erule, enow, frule, fnow;
	frule = (1 << 23) - 1;
	erule = 0xff << 23;
	fnow = uf & frule;
	enow = uf & erule;
	if ((enow == erule) && fnow){
		return uf;
	}
	return (1 << 31) ^ uf;
}
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
    unsigned sign = 0, enow = 0, fnow = 0, absx = x,
	shiftLeft = 0, tail = 0, result = 0;
	unsigned pos = 1 << 31;
	if (x == 0) {
		return 0;
	}else if (x < 0) {
		absx = -x;
		sign = pos;
	}
	while ((pos & absx) == 0) {
		absx <<= 1;
		shiftLeft += 1;
	}
	enow = 127 + 31 - shiftLeft;
	tail = absx & 0xff;
	fnow = (~(pos >> 8)) & (absx >> 8);
	result = sign | (enow << 23) | fnow;
	if (tail > 0x80) {
		result += 1;
	} else if (0x80 == tail) {
		if (fnow & 1) {
		    result += 1;
		}
	}
	return result;	
}
/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
    unsigned sign = 0, enow = 0, fnow = 0;
	unsigned pos = 1 << 31;
	unsigned frule = (1 << 23) - 1;
	if (uf == 0) {
		return 0;
	}
	if (uf == pos) {
		return uf;
	}
	sign = uf & pos;
	enow = (uf >> 23) & 0xff;
	if (enow == 0xff) {
		return uf;
	}
	fnow = uf & frule;
	if (enow == 0) {
		fnow = fnow << 1;
		if (fnow & (1 << 23)) {
			fnow = fnow & frule;
			enow += 1;
		}
	} else {
		enow += 1;
	}
	return sign | (enow << 23) | fnow;
}
