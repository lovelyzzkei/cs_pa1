#include <stdio.h>
#include "bf16.h"

/*
    Do NOT include any C libraries or header files
    except those above
    
    use the defines in bf16.h
    if neccessary, you can add some macros below
*/

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
    printf("round %d guard %d sticky %d\n", round, guard, sticky);

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


    for (int k = 0; k < 32; k++) {
        printf("%d", binaryNum[k]);
    }
    printf("\n");
    for (int k = 0; k < 16; k++) {
        printf("%d", bf16Num[k]);
    }
    printf("\n");


    printf("length of binary representation: %d\n", len);

    printf("Binary representation: 0b");
    for (int j = len - 1; j >= 0; j--) {
        printf("%d", binaryNum[j]);
    }
    printf("\n");

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

    for (int i = 0; i < 16; i++) {
        if (i % 4 == 0) printf(" ");
        printf("%d", bf16Num[i]);
    }
    printf("\n");


    // sign & exponent
    int s = bf16Num[0];
    int exp = 0;
    int bias = 127;
    for (int i = 8; i >= 1; i--) {
        exp += bf16Num[i] * power(2, 8-i);
    }
    int e = exp - bias;
    printf("exponent: %d\n", e);

    // mantissa
    int sticky = 0;
    int guard = 0;
    int round = 0;

    // check round even
    if (e < 7) {
        printf("roundeven\n");
        for (int i = e; i < 7; i++) {
            sticky |= bf16Num[9+i];
        }
        round = bf16Num[9+e];
        guard = bf16Num[8+e];

        if ((round && sticky) || (guard && round && !sticky)) {
            int carry = 1;
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
    for (int i = 0; i < e; i++) {
        result += bf16Num[8+e-i] * power(2, i);
        printf("result: %d\n", result);
    }
    result += power(2, e);
    
    return result;
}

bf16 float2bf16 (float input) {
    bf16 result = 0;
    /*
    fill this function
    */
    return result;
}

float bf162float (bf16 input) {
    float result = 0.0;
    /*
    fill this function
    */
    return result;
}

bf16 bf16_add(bf16 in1, bf16 in2) {
    bf16 result = 0;
    /*
    fill this function
    */
    return result;
}

bf16 bf16_mul(bf16 in1, bf16 in2) {
    bf16 result = 0;
    /*
    fill this function
    */
    return result;

}
