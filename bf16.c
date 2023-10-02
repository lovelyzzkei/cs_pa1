#include <stdio.h>
#include "bf16.h"

/*
    Do NOT include any C libraries or header files
    except those above
    
    use the defines in bf16.h
    if neccessary, you can add some macros below
*/

#define TMAX 2147483647
#define TMIN -2147483648
#define NAN 0x7F81       // 0111 1111 1000 0001 

#define BF16_TMAX 65536 
#define BF16_TMIN 0
#define BF16_POS_INF 0x7F80 // 0111 1111 1000 0000
#define BF16_NEG_INF 0xFF80 // 1111 1111 1000 0000



// Define a union to manipulate the bits of a float
typedef union {
    float f;
    int ii;
    unsigned i;
} Float32;

int nan_flag = 0;

int power(int base, int exp) {
    int result = 1;
    for (int i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}

void fillArray(int* arr, int num, int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = num;
    }
}

bf16 int2bf16 (int input) {
    bf16 result = 0;
    /*
    Function: Convert 32-bit signed integer to 16-bit unsigned integer
        1. Convert 32-bit to normalized form
        2. Truncate the lower 16 bits
    */

    // 1. Convert integer to binary form
    int len = 0;
    int decimalInput = input > 0 ? input : -input;

    // Arrays for store bit representation of bf16 and integer
    int bf16Num[16];
    int binaryNum[32]; // 32-bit binary representation

    fillArray(bf16Num, 0, 16);
    fillArray(binaryNum, 0, 32);
    

    while (decimalInput > 0) {
        binaryNum[31-len] = decimalInput % 2;
        decimalInput = decimalInput / 2;
        len++;
    }

    // If negative integer, just change s of normalized form 
    int s = input > 0 ? 0 : 1;
    int exp = (len - 1) + 127; // Exp = E + bias


    bf16Num[0] = s;

    // Round-even check
    int sticky = 0;
    for (int i = 16; i < 32; i++) {
        sticky |= binaryNum[i];
    }

    int round = binaryNum[15];
    int guard = binaryNum[14];

    int roundEven = 0;
    if ((round &&  sticky) || (guard && round && !sticky)) {
        roundEven = !roundEven;
    }

    // fill mantissa part
    for (int j = 1; j <= 7; j++) {
        if (j >= len) break;
        bf16Num[j+8] = binaryNum[j+32-len];
    }

    if (roundEven) {
        int carry = 1;
        for (int j = 15; j >= 9; j--) {
            int sum = bf16Num[j] + carry;
            bf16Num[j] = sum % 2;
            carry = sum / 2;
        }

        if (carry)
            exp++;
    }

    // fill exp part
    int exp_idx = 8;
    while (exp > 0) {
        bf16Num[exp_idx] = exp % 2;
        exp /= 2;
        exp_idx--;
    }

    int two = 1;
    for (int k = 0; k <= 15; k++) {
        result += bf16Num[15-k] * two;
        two *= 2;
    }

    return result;
}



int bf162int (bf16 input) {
    int result = 0;
    int bf16Num[16];
    int binaryNum[32]; // 32-bit binary representation
    fillArray(bf16Num, 0, 16);
    fillArray(binaryNum, 0, 32);

    // Convert bf16 into binary format array
    for (int i = 0; i < 16; ++i) {
        bf16Num[i] = (input & 0x8000U) ? 1 : 0;
        input <<= 1;
    }

    // check exp = 000...0
    int zeroCheck = 0;
    for (int i = 1; i <= 8; i++) {
        zeroCheck |= bf16Num[i];
    }

    if (!zeroCheck) {
        result = 0;
        return result;
    }

    // sign & exponent
    int sign = bf16Num[0];
    int exp = 0;
    int bias = 127;
    for (int i = 8; i >= 1; i--) {
        exp += bf16Num[i] * power(2, 8-i);
    }
    int e = exp - bias;

    // mantissa
    int sticky = 0;
    int guard = 0;
    int round = 0;
    int carry = 0;

    // check round even
    if (e < 7) {
        for (int i = e; i < 7; i++) {
            sticky |= bf16Num[9+i];
        }
        round = bf16Num[9+e];
        guard = bf16Num[8+e];

        if ((round && sticky) || (guard && round && !sticky)) {
            carry = 1;
            for (int j = 8+e; j > 8; j--) {
                int sum = bf16Num[j] + carry;
                bf16Num[j] = sum % 2;
                carry = sum / 2;
            }

            if (carry)
                e++;
        }
    }

    // need to check possible convert number or not
    int specialCase = 1;
    for (int i = 1; i <= 8; i++) {
        specialCase &= bf16Num[i];
    }

    if (specialCase) {
        if (carry) { // exp=111...1, frac=000...0 -> Infinity
            result = (sign) ? TMIN : TMAX;
        }
        else { // exp=111...1, frac!=000...0 -> NaN
            result = TMIN;
            nan_flag = 1;
        }
    }
    else {
        for (int i = 0; i < e; i++) {
            result += bf16Num[8+e-i] * power(2, i);
        }
        result += power(2, e);
        result = sign ? -result : result;
    }

    return result;
}



bf16 float2bf16 (float input) {
    bf16 result = 0;

    Float32 num;
    num.f = input;

    int bf16Num[16];
    int binaryNum[32]; // 32-bit binary representation
    fillArray(bf16Num, 0, 16);
    fillArray(binaryNum, 0, 32);

    

    for (int j = 0; j < 32; j++) {
        binaryNum[j] = (num.i & 0x80000000) ? 1 : 0;
        num.i <<= 1;
    }

    // sign & exponent
    for (int i = 0; i < 16; i++) {
        bf16Num[i] = binaryNum[i];
    }

    // overflow check
    int overflowCheck = 1;
    for (int i = 1; i <= 7; i++) {
        overflowCheck &= bf16Num[i];
    }

    if (overflowCheck) {
        result = (bf16Num[0]) ? BF16_TMIN : BF16_TMAX;
        return result;
    }


    // mantissa (round-even)
    int sticky = 0;
    int round = 0;
    int guard = 0;

    for (int i = 17; i < 32; i++) {
        sticky |= binaryNum[i];
    }
    round = binaryNum[16];
    guard = binaryNum[15];

    if ((round && sticky) || (guard && round && !sticky)) {
        int carry = 1;
        for (int j = 15; j >= 0; j--) {
            int sum = bf16Num[j] + carry;
            bf16Num[j] = sum % 2;
            carry = sum / 2;
        }

        // if (carry)
        //     bf16Num[9] = 1;
    }

    return result;
}

float bf162float (bf16 input) {
    float result = 0.0;

    Float32 num;
    num.i = input;

    // 16 additional bits after fraction part
    for (int i = 0; i < 16; i++) {
        num.i <<= 1;
    }

    result = num.f;
    return result;
}

bf16 bf16_add(bf16 in1, bf16 in2) {
    bf16 result = 0;

    nan_flag = 0;
    
    int b1 = bf162int(in1);
    int b2 = bf162int(in2);

    if (b1 == TMAX) {
        if (b2 == TMIN) result = NAN;
        else result = BF16_POS_INF;
    }
    else if (b1 == TMIN) {
        result = BF16_NEG_INF;
    }
    else if (nan_flag) {
        result = NAN;
    }
    else {
        int sum = b1 + b2;
        result = int2bf16(sum);
    }

    return result;
}

bf16 bf16_mul(bf16 in1, bf16 in2) {
    bf16 result = 0;

    nan_flag = 0;

    int b1 = bf162int(in1);
    int b2 = bf162int(in2);

    if (b1 == TMAX) {
        if (b2 == 0) result = NAN;
        else if (b2 == TMAX || b2 > 0) 
            result = BF16_POS_INF;
        else
            result = BF16_NEG_INF;
    }
    else if (b1 == TMIN) {
        if (b2 == 0)
            result = NAN;
        else if (b2 == TMIN || b2 < 0)
            result = BF16_POS_INF;
        else
            result = BF16_NEG_INF;
    }
    else if (nan_flag) {
        result = NAN;
    }
    else {
        int mul = b1 * b2;
        result = int2bf16(mul);
    }

    return result;
}
