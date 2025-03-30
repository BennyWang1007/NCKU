"loop:\n\t"
    "lw t0, 0(%[h])\n\t"        // t0 = p_h[i]
    "addi %[lw_cnt], %[lw_cnt], 1\n\t"
    "lw t1, 0(%[x])\n\t"        // t1 = p_h[i]
    "addi %[lw_cnt], %[lw_cnt], 1\n\t"
    "mul t0, t0, t1\n\t"        // t0 = p_h[i] * p_x[i]
    "addi %[mul_cnt], %[mul_cnt], 1\n\t"
    "add t0, t0, %[id]\n\t"      // t0 = p_h[i] * p_x[i] + id
    "addi %[add_cnt], %[add_cnt], 1\n\t"
    "sw t0, 0(%[y])\n\t"        // p_y[i] = p_h[i] * p_x[i] + id
    "addi %[sw_cnt], %[sw_cnt], 1\n\t"
    "addi %[h], %[h], 2\n\t"        // p_h++
    "addi %[add_cnt], %[add_cnt], 1\n\t"
    "addi %[x], %[x], 2\n\t"        // p_x++
    "addi %[add_cnt], %[add_cnt], 1\n\t"
    "addi %[y], %[y], 2\n\t"        // p_y++
    "addi %[add_cnt], %[add_cnt], 1\n\t"
    "addi %[arr_size], %[arr_size], -1\n\t"
    "addi %[add_cnt], %[add_cnt], 1\n\t"
    "addi %[others_cnt], %[others_cnt], 1\n\t"
    "bne %[arr_size], x0, loop\n\t" // if i < arr_size, goto loop