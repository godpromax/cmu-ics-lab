/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
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
int getByte(int x, int n) {
    return (x>>(n<<3))&0xff;
}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
    int mask;
  	x>>=n;
    mask=(((~(1<<31))>>n)<<1)|1;
    return x&mask;
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
    int mask_1,mask_2,mask_4,mask_8,mask_16;
  	mask_1=0x55|(0x55<<8);
  	mask_1=mask_1|(mask_1<<16);
  	mask_2=0x33|(0x33<<8);
  	mask_2=mask_2|(mask_2<<16);
  	mask_4=0x0f|(0x0f<<8);
  	mask_4=mask_4|(mask_4<<16);
  	mask_8=0xff|(0xff<<16);
  	mask_16=0xff|(0xff<<8);
  	x=(x&mask_1)+((x>>1)&mask_1);
  	x=(x&mask_2)+((x>>2)&mask_2);
  	//下面三组最高位不会是1了 
  	x=(x+(x>>4))&mask_4;//x=(x&mask_4)+((x>>4)&mask_4);
  	x=(x+(x>>8))&mask_8;//x=(x&mask_8)+((x>>8)&mask_8);
  	x=(x+(x>>16))&mask_16;//x=(x&mask_16)+((x>>16)&mask_16);
  	return x;
}
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
    int mask=~x+1;//x的补码
    return ~((x|mask)>>31)&1; //移位后进行位反操作再&1，得到正确映射关系
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
    return 0x80<<24;
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
    int shiftnumber=32+~n+1;//32-n
    int xx=x<<shiftnumber>>shiftnumber;
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
int divpwr2(int x, int n) {
    int mask=x>>31;
    int bias=mask&((1<<n)+(~0));//x为负数时temp为(1<<n)-1，x为正数（或0）时temp为0 
    return (x+bias)>>n;
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
int isPositive(int x) {
    return !((!x)|(x>>31));
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    int signx=(x>>31)&1;//x的符号位 
  	int signy=(y>>31)&1;//y的符号位 
  	int signy_x=((y+~x+1)>>31)&1;//y-x的符号位 
  	return (!(signx^signy)&!signy_x) | ((signx^signy)&signx);//同号 异号 
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
   int sign,shift1,shift2,shift3,shift4,shift5;
  	sign=!!(x>>16);//若x表示的二进制数左边16位没有1，sign为0,有1则sign为1 
  	shift1=sign<<4;//0^4 or 2^4
	x=x>>shift1;//若sign为1，丢掉右边16位，因为1必定出现在左边16位中 
	
	sign=!!(x>>8);//若x表示的二进制数左边8位没有1，sign为0,有1则sign为1 
	shift2=sign<<3;//0^3 or 2^3
	x=x>>shift2;//若sign为1，丢掉右边8位，因为1必定出现在左边8位中 
	
	sign=!!(x>>4);//若x表示的二进制数左边4位没有1，sign为0,有1则sign为1 
	shift3=sign<<2;//0^2 or 2^2
	x=x>>shift3;//若sign为1，丢掉右边4位，因为1必定出现在左边4位中 
	
	sign=!!(x>>2);//若x表示的二进制数左边2位没有1，sign为0,有1则sign为1 
	shift4=sign<<1;//0^1 or 2^1
	x=x>>shift4;//若sign为1，丢掉右边2位，因为1必定出现在左边2位中 
	
	sign=!!(x>>1);//若x表示的二进制数左边1位没有1，sign为0,有1则sign为1 
	shift5=sign; 
	
	return shift1+shift2+shift3+shift4+shift5;//偏移量相加即得结果 
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
 //在最高位置为0情况下，如果大于0x7F800000，一定是NaN，直接返回。如果不大于符号为取反，返回。
 unsigned tmp;
 unsigned result;
 result = uf ^ (0x80 <<24);	//让符号位取反
 //如果最高位置为0时，需要判断它与0x7F800000的大小
 tmp = uf & 0x7FFFFFFF;
 if(tmp > 0x7F800000) result = uf;
 return result;
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
  //注意这里定义的是unsigned，右移和左移都会是逻辑移位，即补0 
	unsigned absx,sign,flag,temp,aftershift,shiftnum;
	absx=x;
	sign=x&(1<<31);//符号位
	if(sign) absx=-x;
	aftershift=absx; 
	if(!x) return 0; 
	shiftnum=0;
	//左移shiftnum-1位 
	while(1)
	{
		temp=aftershift;
		//aftershift要多移一位，aftershift的高位往低位数共23位即float的尾码部分 
		aftershift=aftershift<<1;
		shiftnum++;
		if(temp&0x80000000) break;//1000 0000 0000…… ，最高位为1 
	}
	//aftershift的前23位要充当float的尾码部分，四舍五入，判断进位 
  	if((aftershift&0x01ff)>0x0100)  flag=1;//后9位中 第1位为1，后8位有1 
	else if((aftershift&0x03ff)==0x0300) flag=1;//后9位中 第一位为1，后8位为0
	else flag=0;
	return  sign+((159-shiftnum)<<23)+(aftershift>>9)+flag;

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
  if((uf&0x7f800000)==0)  //阶码为0，非规格化数
		uf=((uf&0x007fffff)<<1)|(uf&0x80000000);//尾数域左移一位，别忘了符号
	else if((uf&0x7f800000)!=0x7f800000) //不是NaN ，阶码不是全1 
		uf=uf+0x800000;//阶码+1，2^(1+1)=2*2 
	//NaN，就地返回吧 
  return uf;
}
