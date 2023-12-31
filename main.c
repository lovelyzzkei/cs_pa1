#include <stdio.h>
#include "bf16.h"

void put_bf16(bf16 in)
{
    int i;
    for ( i=0; i < 16; ++i ) {
        putchar((in & 0x8000U) ? '1' : '0');
        in <<= 1;
    }
}

int main(void)
{
    int input1;
    bf16 input2;
    float input3;
    bf16 input4;
    bf16 input5, input6;
    bf16 input7, input8;
    union {float f; unsigned u;} result4;

    scanf("%d\n", &input1);
    printf("int2bf16(%d) = 0x%04hx\n", input1, int2bf16(input1));
    scanf("%hx\n", &input2);
    printf("bf162int(0x%04hx) = %d\n", input2, bf162int(input2));
    scanf("%f\n", &input3);
    printf("float2bf16(%f) = 0x%04hx\n", input3, float2bf16(input3));
    scanf("%hx\n", &input4);
    result4.f = bf162float(input4);
    printf("bf162float(%04hx) = %f (0x%08x)\n", input4, result4.f, result4.u);
    scanf("%hx %hx\n", &input5, &input6);
    printf("bf16_add(0x%04hx, 0x%04hx) = 0x%04hx\n", input5, input6, bf16_add(input5, input6));
    scanf("%hx %hx\n", &input7, &input8);
    printf("bf16_mul(0x%04hx, 0x%04hx) = 0x%04hx\n", input7, input8, bf16_mul(input7, input8));

    printf("put_bf16(0x%04hx) is ", input1);
    put_bf16(input1);
    putchar('\n');

    return 0;
}

