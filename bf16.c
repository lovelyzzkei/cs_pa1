#include <stdio.h>
#include <math.h>
#include <memory.h>
#include "bf16.h"

/*
    Do NOT include any C libraries or header files
    except those above
    
    use the defines in bf16.h
    if neccessary, you can add some macros below
*/

bf16 int2bf16 (int input) {
    bf16 result = 0;
    /*
    Function: Convert 32-bit signed integer to 16-bit unsigned integer
        1. Convert 32-bit to normalized form
        2. Truncate the lower 16 bits
    */

    // 1. Convert integer to binary form
    int i = 0;
    int decimalInput = input > 0 ? input : -input;
    int binaryNum[32]; // 32-bit binary representation
    memset(binaryNum, 0, 32*sizeof(int));

    while (decimalInput > 0) {
        binaryNum[31-i] = decimalInput % 2;
        decimalInput = decimalInput / 2;
        i++;
    }

    // If negative integer, change to two's complement representation
    if (input < 0) {
        for (int j = 0; j < 32; j++) {
            binaryNum[j] = !binaryNum[j];
        }
        int carry = 1;
        for (int j = 0; j < 32; j++) {
            int sum = binaryNum[j] + carry;
            binaryNum[j] = sum % 2;
            carry = sum / 2;
        }

        if (carry && i < 31) {
            binaryNum[++i] = 1;
        }
    }

    int s = binaryNum[0];
    int exp = (i - 1) + 127; // Exp = E + bias
    
    int j = 8;
    int bf16Num[16];
    memset(bf16Num, 0, 16*sizeof(int));

    bf16Num[0] = s;

    // fill exp part
    while (exp > 0) {
        bf16Num[j] = exp % 2;
        exp /= 2;
        j--;
    }

    // fill mantissa part (round-even)
    for (j = 1; j <= 7; j++) {
        if (j >= i) break;
        bf16Num[j+8] = binaryNum[j+32-i];
    }

    for (int k = 0; k < 32; k++) {
        printf("%d", binaryNum[k]);
    }
    printf("\n");
    for (int k = 0; k < 16; k++) {
        printf("%d", bf16Num[k]);
    }
    printf("\n");



    


    printf("length of binary representation: %d\n", i);




    printf("Binary representation: 0b");
    for (int j = i - 1; j >= 0; j--) {
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
    /*
    fill this function
    */
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
