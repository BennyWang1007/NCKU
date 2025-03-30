"addi t4, x0, 0\n\t"                        // i = 0
"addi %[add_cnt], %[add_cnt], 1\n\t"
"li t1, -1\n\t"                            // t1 = -1
"addi %[lw_cnt], %[lw_cnt], 1\n\t"
"loop:\n\t"
    "li t3, -1\n\t"                            // t3 = -1
    "addi %[lw_cnt], %[lw_cnt], 1\n\t"
    "mul t1, t1, t3\n\t"                        // t1 = (-1)^i
    "addi %[mul_cnt], %[mul_cnt], 1\n\t"
    "slli t2, t4, 1\n\t"                        // t2 = 2 * i
    "addi %[others_cnt], %[others_cnt], 1\n\t"
    "addi t2, t2, 1\n\t"                        // t2 = 2 * i + 1
    "addi %[add_cnt], %[add_cnt], 1\n\t"
    "fcvt.d.w f1, t1\n\t"                       // f1 = (float)t1
    "addi %[others_cnt], %[others_cnt], 1\n\t"
    "fcvt.d.w f2, t2\n\t"                       // f2 = (float)t2
    "addi %[others_cnt], %[others_cnt], 1\n\t"
    "fdiv.d f1, f1, f2\n\t"                     // f1 = f1 / f2
    "addi %[div_cnt], %[div_cnt], 1\n\t"
    "fadd.d %[pi], %[pi], f1\n\t"               // pi += f1
    "addi %[add_cnt], %[add_cnt], 1\n\t"
    "addi t4, t4, 1\n\t"                        // i++
    "addi %[add_cnt], %[add_cnt], 1\n\t"
    "addi %[others_cnt], %[others_cnt], 1\n\t"
    "blt t4, %[N], loop\n\t"                    // if i < N, goto loop
    