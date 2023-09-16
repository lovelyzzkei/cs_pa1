#ifndef BF16_H
#define BF16_H

typedef unsigned short bf16;

bf16 int2bf16(int input);
int  bf162int(bf16 input);
bf16 float2bf16(float input);
float bf162float(bf16 input);

bf16 bf16_add(bf16 in1, bf16 in2);
bf16 bf16_mul(bf16 in1, bf16 in2);

// Helper function to output binary pattern of bf16 to stdin
// Be careful! This function doesn't put '\n' to stdin. 
extern void put_bf16(bf16 in);

#endif
