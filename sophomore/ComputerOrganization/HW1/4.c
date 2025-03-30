#include<stdio.h>

int main()
{ 
    int i = 0;
    int h[9] = {0}, x[6] = {0}, y[6] = {0}; 
    FILE *input = fopen("../input/4.txt", "r");
    for(i = 0; i < 9; i++) fscanf(input, "%d", &h[i]);
    for(i = 0; i < 6; i++) fscanf(input, "%d", &x[i]);
    for(i = 0; i < 6; i++) fscanf(input, "%d", &y[i]);
    fclose(input);
    
    int *p_x = &x[0];
    int *p_h = &h[0];
    int *p_y = &y[0];
    
    asm volatile(
        "li t4, 3\n\t"                   // Initialize t4 = h.length
        "li t5, 2\n\t"                   // Initialize t5 = x[0].length
        "li t6, 3\n\t"                   // Initialize t6 = h[0].length
        "li %[i], 0\n\t"                 // Initialize i = 0
        "loop_i:\n\t"
        "bge %[i], t4, loop_i_end\n\t"   // Check if i >= h.length(3)
        "li %[j], 0\n\t"                 // Initialize j = 0
        "loop_j:\n\t"
        "bge %[j], t5, loop_j_end\n\t"   // Check if j >= x[0].length(2)
        "li %[f], 0\n\t"                 // Initialize f = 0
        "loop_f:\n\t"
        "bge %[f], t6, loop_f_end\n\t"   // Check if f >= h[0].length(3)

        // code of p3
        "mul t0, %[i], t6\n\t"   // t0 = i * 3
        "add t0, t0, %[f]\n\t"   // t0 = i * 3 + f
        "slli t0, t0, 2\n\t"     // t0 = (i * 3 + f) * 4
        "add a0, %[p_h], t0\n\t" // a0 = &h[i * 3 + f]
        "lw t0, 0(a0)\n\t"       // load h[i * 3 + f] into t0

        "mul t1, %[f], t5\n\t"   // t1 = f * 2
        "add t1, t1, %[j]\n\t"   // t1 = f * 2 + j
        "slli t1, t1, 2\n\t"     // t1 = (f * 2 + j) * 4
        "add a1, %[p_x], t1\n\t" // a1 = &x[f * 2 + j]
        "lw t1, 0(a1)\n\t"       // load x[f * 2 + j] into t1

        "mul t2, %[i], t5\n\t"   // t2 = i * 2
        "add t2, t2, %[j]\n\t"   // t2 = i * 2 + j
        "slli t2, t2, 2\n\t"     // t2 = (i * 2 + j) * 4
        "add a2, %[p_y], t2\n\t" // a2 = &y[i * 2 + j]
        "lw t2, 0(a2)\n\t"       // load y[i * 2 + j] into t2

        "mul t0, t0, t1\n\t"     // t0 = h[i * 3 + f] * x[f * 2 + j]
        "add t0, t0, t2\n\t"     // t0 = product + y[i * 2 + j]
        "sw t0, 0(a2)\n\t"       // store y[i * 2 + j] into t0
        // end of code of p3

        "addi %[f], %[f], 1\n\t"    // Increment f
        "j loop_f\n\t"
        "loop_f_end:\n\t"
        "addi %[j], %[j], 1\n\t"    // Increment j
        "j loop_j\n\t"
        "loop_j_end:\n\t"
        "addi %[i], %[i], 1\n\t"    // Increment i
        "j loop_i\n\t"
        "loop_i_end:\n\t"
        : [i] "+r" (i), [j] "+r" (i), [f] "+r" (i), [p_x] "+r" (p_x), [p_h] "+r" (p_h), [p_y] "+r" (p_y)
        : 
        : "a0", "a1", "a2", "t0", "t1", "t2", "t3", "t4", "t5", "t6"
        // a0: address of h[i * 3 + f], a1: address of x[f * 2 + j], a2: address of y[i * 2 + j]
        // t0: h[i * 3 + f], t1: x[f * 2 + j], t2: y[i * 2 + j], t3: tmp for ops
        // t4: h.length, t5: x[0].length, t6: h[0].length
    );

    p_y = &y[0];
    for(i = 0; i < 6; i++)
        printf("%d ", *p_y++);
    printf("\n");
    return 0; 
 
}
