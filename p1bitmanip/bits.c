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
//#include "absVal.c"
//#include "addOK.c"
//#include "allEvenBits.c"
//#include "allOddBits.c"
//#include "anyEvenBit.c"


/* 
 * anyOddBit - return 1 if any odd-numbered bit in word set to 1
 *   Examples anyOddBit(0x5) = 0, anyOddBit(0x7) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int anyOddBit(int x) {
    /* we are checking each byte individually (there are 4 bytes)
     * 0xAA == 10101010 so when we & each byte with 0xAA, the even
     * bits are all masked to 0 and if the remaining result is not 0
     * then we know that at least one odd_numbered bit is 1
    */
    return !!((x & 0xAA) | (x >> 8 & 0xAA) |
              (x >> 16 & 0xAA) | (x >> 24 & 0xAA));
}
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
    /* out of all integers, only 0 has the same sign but as its
     * opposite number (~0 + 1). Therefore, taking advantage of this
     * only (0 >> 31) | ((~0 + 1) >> 31)) == 0. Since we want to return
     * 1 when x = 0, so we added 1 at the end. With the same logic, any
     * non-zero integer will return -1 + 1 = 0.
     */
    return ((x >> 31) | ((~x + 1) >> 31)) + 1;
}
//#include "bitAnd.c"
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
    /* The general strategy here is to create five masks
     * to help add two bits, pair of bits, group of four bits,
     * group of eight bits, and group of sixteen bits.
     * to do that we first mask with x and then shift x to the 
     * right accordingly. 
     */
    int count, mask1_tmp, mask2_tmp, mask3_tmp,
        mask1, mask2, mask3, mask4, mask5;

    // Adding two bits 
    // Making 0x55555555
    mask1_tmp = 0x55 | (0x55 << 8);
    mask1 = mask1_tmp | (mask1_tmp << 16);
    count = (x & mask1) + ((x >> 1) & mask1);

    // Adding pair of bits
    // Making 0x33333333
    mask2_tmp = 0x33 | (0x33 << 8);
    mask2 = mask2_tmp | (mask2_tmp << 16);
    count = (count & mask2) + ((count >> 2) & mask2);

    // Adding group of four bits
    // Making 0x0F0F0F0F
    mask3_tmp = 0x0F | (0x0F << 8);
    mask3 = mask3_tmp | (mask3_tmp << 16);
    count = (count & mask3) + ((count >> 4) & mask3);

    // Adding group of eight bits
    // Making 0x00FF00FF
    mask4 = 0xFF | (0xFF << 16);
    count = (count & mask4) + ((count >> 8) & mask4);

    // Adding group of sixteen bits
    // Making 0x0000FFFF
    mask5 = 0xFF | (0xFF << 8);
    count = (count & mask5) + ((count >> 16) & mask5);

    return count;
}
/* 
 * bitMask - Generate a mask consisting of all 1's 
 *   lowbit and highbit
 *   Examples: bitMask(5,3) = 0x38
 *   Assume 0 <= lowbit <= 31, and 0 <= highbit <= 31
 *   If lowbit > highbit, then mask should be all 0's
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int bitMask(int highbit, int lowbit) {
    /* can't move highbit + 1 because highbit can be 31
     * and in that case moving 32 will lead to the last
     * bit move being unsure
     */
    int first, second;
    first = ~0 << (highbit);
    first = first << 1;
    second = ~(~0 << lowbit);
    return ~(first | second);
}
//#include "bitNor.c"
//#include "bitOr.c"
/*
 * bitParity - returns 1 if x contains an odd number of 0's
 *   Examples: bitParity(5) = 0, bitParity(7) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int bitParity(int x) {
    /* Notice that XOR operation preserves parity
     * In a nibble, if we have odd number of 1's, 
     * the parity is 1, and zero otherwise. Also
     * notice that counting odd number of 1's is the 
     * same as counting odd number of 0's
     */
    x = x ^ (x >> 16);
    x = x ^ (x >> 8);
    x = x ^ (x >> 4);
    x = x ^ (x >> 2);
    x = x ^ (x >> 1);
    x = x & 1;
    return x;
}
//#include "bitXor.c"
//#include "byteSwap.c"
//#include "conditional.c"
//#include "copyLSB.c"
//#include "divpwr2.c"
//#include "evenBits.c"
//#include "ezThreeFourths.c"
//#include "fitsBits.c"
//#include "fitsShort.c"
//#include "float_abs.c"
//#include "float_f2i.c"
//#include "float_half.c"
//#include "float_i2f.c"
//#include "float_neg.c"
//#include "float_twice.c"
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
    /*
    Given n, we want to shift x to the right,
    since each n represents a byte, so the total shift amount
    to the right to move the wanted byte to far right is to 
    apply (n << 3) <==> n* (2^3) right shift.
    Then & with 0xFF to extract the byte.
    */
    return (x >> (n << 3)) & 0xFF;
}
//#include "greatestBitPos.c"
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
    /* The first step is to turn x into a positive number, so
     * the sign bit (leading bit) is 0. Now we would like to
     * count the number of leading 0s. With ~, we can count the
     * number of leading 1s, which is the exact same problem as
     * leftBitCount/bitCount.
     */
    int count, mask1_tmp, mask2_tmp, mask3_tmp, mask1, mask2,
        mask3, mask4, mask5, sign_x, pos_x, new_x;

    // this operation turns x into positive
    sign_x = x >> 31;
    pos_x = x ^ sign_x;

    // count number of 1s in the front
    // now just implement left bit count 
    new_x = ~pos_x;

    x = new_x;

    x &= x >> 1;
    x &= x >> 2;
    x &= x >> 4;
    x &= x >> 8;
    x &= x >> 16;

    // Adding two bits 
    // Making 0x55555555
    mask1_tmp = 0x55 | (0x55 << 8);
    mask1 = mask1_tmp | (mask1_tmp << 16);
    count = (x & mask1) + ((x >> 1) & mask1);

    // Adding pair of bits
    // Making 0x33333333
    mask2_tmp = 0x33 | (0x33 << 8);
    mask2 = mask2_tmp | (mask2_tmp << 16);
    count = (count & mask2) + ((count >> 2) & mask2);

    // Adding group of four bits
    // Making 0x0F0F0F0F
    mask3_tmp = 0x0F | (0x0F << 8);
    mask3 = mask3_tmp | (mask3_tmp << 16);
    count = (count & mask3) + ((count >> 4) & mask3);

    // Adding group of eight bits
    // Making 0x00FF00FF
    mask4 = 0xFF | (0xFF << 16);
    count = (count & mask4) + ((count >> 8) & mask4);

    // Adding group of sixteen bits
    // Making 0x0000FFFF
    mask5 = 0xFF | (0xFF << 8);
    count = (count & mask5) + ((count >> 16) & mask5);

    // takes care of the sign bit with the last +1
    return 32 + (~count + 1) + 1;
}
//#include "ilog2.c"
//#include "implication.c"
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
    /* true only when all three of the following conditions hold true:
     * 1) x is positive, so sign_x == 0
     * 2) x - 0x30 is positive
     * 3) 0x39 - x is positive
     */
    int sign_x = x >> 31 & 1;
    int diff_1 = x + (~0x30 + 1);
    int diff_2 = 0x39 + (~x + 1);

    // sign of the first diff will be 0 when x >= 0x30
    int sign_diff_1 = diff_1 >> 31 & 1;
    int sign_diff_2 = diff_2 >> 31 & 1;
    return !(sign_x | sign_diff_1 | sign_diff_2);
}
/* 
 * isEqual - return 1 if x == y, and 0 otherwise 
 *   Examples: isEqual(5,5) = 1, isEqual(4,5) = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int isEqual(int x, int y) {
    /* Recall XOR gives 0 when the bits are equal */
    return !(x ^ y);
}
/* 
 * isGreater - if x > y  then return 1, else return 0 
 *   Example: isGreater(4,5) = 0, isGreater(5,4) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isGreater(int x, int y) {
    // Comments are throughout the code.
    int sign_x, sign_y, diff, sign_diff, sign_equal, sign_not_equal;
    // negative: 1
    // positive: 0
    sign_x = (x >> 31) & 1;
    sign_y = (y >> 31) & 1;

    // diff = x - y
    // x > y = x >= y + 1
    diff = x + (~y);
    sign_diff = (diff >> 31) & 1;

    // when the signs of x and y are equal: NOT overflow
    // sign_equal will be the sign of sign_diff when the signs are equal (no overflow)
    // sign_euqal will be 0 when the signs are not equal (possibly overflow)
    sign_equal = (!(sign_x ^ sign_y)) & sign_diff;
    // when the signs of x and y are NOT equal: possiblity of overflow
    // sign_not_equal will be the sign of sign_x when the signs are not equal
    // sign_not_equal will be 0 when the signs are equal
    sign_not_equal = sign_x & (!sign_y);

    return !(sign_equal | sign_not_equal);
}
//#include "isLess.c"
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    int sign_x, sign_y, diff, sign_diff, sign_equal, sign_not_equal;
    // negative: 1
    // positive: 0
    sign_x = (x >> 31) & 1;
    sign_y = (y >> 31) & 1;

    // diff = y - x
    // y >= x
    diff = y + (~x + 1);
    sign_diff = (diff >> 31) & 1;

    sign_equal = (!(sign_x ^ sign_y)) & sign_diff;
    sign_not_equal = sign_y & (!sign_x);

    return !(sign_equal | sign_not_equal);
}
//#include "isNegative.c"
/* 
 * isNonNegative - return 1 if x >= 0, return 0 otherwise 
 *   Example: isNonNegative(-1) = 0.  isNonNegative(0) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 3
 */
int isNonNegative(int x) {
    /*
    Using the fact that
    MSB is 1 ==> negative
    MSB is 0 ==> non-negative
    & with 1 isolates all bits other than the LSB
    negation in front used to satisfy the required return value
    */
    return !((x >> 31) & 1);
}
//#include "isNonZero.c"
/* 
 * isNotEqual - return 0 if x == y, and 1 otherwise 
 *   Examples: isNotEqual(5,5) = 0, isNotEqual(4,5) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int isNotEqual(int x, int y) {
    /* Same logic as isEqual, except need one more negation. */
    return !!(x ^ y);
}
//#include "isPositive.c"
//#include "isPower2.c"
//#include "isTmax.c"
//#include "isTmin.c"
//#include "isZero.c"
//#include "leastBitPos.c"
/*
 * leftBitCount - returns count of number of consective 1's in
 *     left-hand (most significant) end of word.
 *   Examples: leftBitCount(-1) = 32, leftBitCount(0xFFF0F0F0) = 12
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 50
 *   Rating: 4
 */
int leftBitCount(int x) {
    /* Here we first change all bits after the first 0 bit from left
     * to be 0's, then we use count bit code on it.
     */
    int count, mask1_tmp, mask2_tmp, mask3_tmp,
        mask1, mask2, mask3, mask4, mask5;

    // Changing all bits to 0 after the first 0 bit from left.
    x &= x >> 1;
    x &= x >> 2;
    x &= x >> 4;
    x &= x >> 8;
    x &= x >> 16;

    // Using bitCount code here.
    // Adding two bits 
    mask1_tmp = 0x55 | (0x55 << 8);
    mask1 = mask1_tmp | (mask1_tmp << 16);
    count = (x & mask1) + ((x >> 1) & mask1);

    // Adding pair of bits
    mask2_tmp = 0x33 | (0x33 << 8);
    mask2 = mask2_tmp | (mask2_tmp << 16);
    count = (count & mask2) + ((count >> 2) & mask2);

    // Adding group of four bits
    mask3_tmp = 0x0F | (0x0F << 8);
    mask3 = mask3_tmp | (mask3_tmp << 16);
    count = (count & mask3) + ((count >> 4) & mask3);

    // Adding group of eight bits
    mask4 = 0xFF | (0xFF << 16);
    count = (count & mask4) + ((count >> 8) & mask4);

    // Adding group of sixteen bits
    mask5 = 0xFF | (0xFF << 8);
    count = (count & mask5) + ((count >> 16) & mask5);

    return count;
}
//#include "logicalNeg.c"
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
    /*
    Recall (x >> n) performs arithmetic shift, for example,
    if we have 10001000 and we shift 4 bits to the right, we get 11111000.
    To convert to 00001000, we need to & it with 00001111 (preserve right four bits
    and change left four bits to 0000).
    */
    return (x >> n) & ~((~0 << (32 + ~n)) << 1);
}
//#include "minusOne.c"
//#include "multFiveEighths.c"
//#include "negate.c"
//#include "oddBits.c"
/* 
 * rempwr2 - Compute x%(2^n), for 0 <= n <= 30
 *   Negative arguments should yield negative remainders
 *   Examples: rempwr2(15,2) = 3, rempwr2(-35,3) = -3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int rempwr2(int x, int n) {
    /* Comments are throughout the code. */
    int result, mask, subtract;
    // Representing modulo: x % y = x & (2^y - 1) 
    result = x & ((1 << n) + ~0);

    // Dealing with negative/zero case
    // we need to subtract 2^n from the result
    // the !!(result) takes care of positive = 0 case
    mask = x >> 31;
    subtract = ~((!!result) << n) + 1;

    return result + (subtract & mask);
}
/* 
 * replaceByte(x,n,c) - Replace byte n in x with c
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: replaceByte(0x12345678,1,0xab) = 0x1234ab78
 *   You can assume 0 <= n <= 3 and 0 <= c <= 255
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 3
 */
int replaceByte(int x, int n, int c) {
    /* 
    First step is to create a mask to unset the desired byte, ie. x & mask
    to do this, we shift 0xFF to left by (n * 8) and inverse it. Next, 
    we shift c to left by (n * 8) and OR it with (x & mask).
    */
    int mask;
    mask = ~(0xFF << (n << 3));
    return (x & mask) | (c << (n << 3));
}
//#include "reverseBytes.c"
//#include "rotateLeft.c"
/* 
 * rotateRight - Rotate x to the right by n
 *   Can assume that 0 <= n <= 31
 *   Examples: rotateRight(0x87654321,4) = 0x76543218
 *   Legal ops: ~ & ^ | + << >> !
 *   Max ops: 25
 *   Rating: 3 
 */
int rotateRight(int x, int n) {
    /* The mask masks the leading n bits which are attained with the first
     * operation >> -- we dont want them but to replace them with the n bits
     * that are shifted away. This is when shifted comes into play because it
     * it copies those n bits into the right positiion and ultimate by returning
     * first OR shifted, we have the right answer
     */
    int first, mask, shifted;
    first = (x >> n);
    mask = ~ (1 << 31 >> (n + ~0));
    first = first & mask;
    shifted = x << (32 + (~n + 1));
    return first | shifted;
}
//#include "satAdd.c"
//#include "satMul2.c"
/*
 * satMul3 - multiplies by 3, saturating to Tmin or Tmax if overflow
 *  Examples: satMul3(0x10000000) = 0x30000000
 *            satMul3(0x30000000) = 0x7FFFFFFF (Saturate to TMax)
 *            satMul3(0x70000000) = 0x7FFFFFFF (Saturate to TMax)
 *            satMul3(0xD0000000) = 0x80000000 (Saturate to TMin)
 *            satMul3(0xA0000000) = 0x80000000 (Saturate to TMin)
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 3
 */
int satMul3(int x) {
    /* By comparing the signs of x, 2x and 3x, we know when an overflow takes
     * place or not. There will be an over flow when the sign of x and 2x differ
     * Thus, when sign_x | sign_two_x == 1, we know that there is an overflow.
     * Same idea for sign_two_x and sign_three_x. Thus, when either of the above conditions
     * satisfy (use |), we know there is an overflow. 
     */
    int two_x, three_x, sign_x, sign_two_x, sign_three_x,
        one_two, two_three, overflow, Tmin, Tmax;

    Tmin = 1 << 31;
    Tmax = ~Tmin;

    two_x = x + x;
    three_x = x + two_x;

    // 1111...1111 if x is negative
    // 0000...0000 if x is non-negative
    sign_x = x >> 31;
    sign_two_x = two_x >> 31 & 1;
    sign_three_x = three_x >> 31 & 1;

    // if one_two is 1, then there will be overflow
    one_two = (sign_x & 1) ^ sign_two_x;
    // if two_three is 1, then there will be overflow
    two_three = sign_two_x ^ sign_three_x;
    // 1 if there is an overflow
    // 0 if there is no overflow
    overflow = one_two | two_three;
    // 1111...1111 if there is an overflow
    // 0000...0000 if there is no overflow
    overflow = overflow << 31 >> 31;

    return ((~overflow) & three_x) |
           (overflow & (~sign_x) & Tmax) |
           (overflow & sign_x & Tmin);
}
//#include "sign.c"
//#include "sm2tc.c"
/* 
 * subOK - Determine if can compute x-y without overflow
 *   Example: subOK(0x80000000,0x80000000) = 1,
 *            subOK(0x80000000,0x70000000) = 0, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int subOK(int x, int y) {
    /* We know that there is an overflow only when the signs of x and y differ
     * AND when the sign of x and the sign of (x-y) differ.
     */
    int sign_x, sign_y, diff, sign_diff;
    // overflow is only possible when:
    // (1) sign_x and sign_y are different
    // (2) sign_x and sign_diff are different
    sign_x = (x >> 31) & 1;
    sign_y = (y >> 31) & 1;

    // diff = x - y
    diff = x + (~y + 1);
    sign_diff = (diff >> 31) & 1;

    // sign_x and sign_y are the same, thus no overflow is possible
    return !((sign_x ^ sign_y) & (sign_x ^ sign_diff));
}
//#include "tc2sm.c"
//#include "thirdBits.c"
//#include "tmax.c"
//#include "tmin.c"
//#include "trueFiveEighths.c"
//#include "trueThreeFourths.c"
//#include "upperBits.c"
