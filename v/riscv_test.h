// See LICENSE for license details.

#ifndef _ENV_VIRTUAL_SINGLE_CORE_H
#define _ENV_VIRTUAL_SINGLE_CORE_H

#include "../p/riscv_test.h"

//-----------------------------------------------------------------------
// Begin Macro
//-----------------------------------------------------------------------

#undef RVTEST_FP_ENABLE
#define RVTEST_FP_ENABLE fssr x0

#undef RVTEST_VECTOR_ENABLE
#define RVTEST_VECTOR_ENABLE                                            \
  csrwi fcsr, 0;                                                        \
  csrwi vcsr, 0;

#undef RVTEST_CODE_BEGIN
#define RVTEST_CODE_BEGIN                                               \
        .data;                                                          \
        .global test_failed_info;                                       \
test_failed_info:                                                       \
        .string  "failed";                                               \
        .text;                                                          \
        .global extra_boot;                                             \
extra_boot:                                                             \
        EXTRA_INIT                                                      \
        ret;                                                            \
        .global userstart;                                              \
userstart:                                                              \
        init

//-----------------------------------------------------------------------
// Pass/Fail Macro
//-----------------------------------------------------------------------

#undef RVTEST_PASS
#define RVTEST_PASS li a0, 0;li a7, 1; scall

#undef RVTEST_FAIL
#define RVTEST_FAIL lui a0, %hi(test_failed_info);addi a0, a0, %lo(test_failed_info);li a1, 6;li a7, 4; scall;

//-----------------------------------------------------------------------
// Data Section Macro
//-----------------------------------------------------------------------

#undef RVTEST_DATA_END
#define RVTEST_DATA_END

//-----------------------------------------------------------------------
// Supervisor mode definitions and macros
//-----------------------------------------------------------------------

#define MAX_TEST_PAGES 63 // this must be the period of the LFSR below
#define LFSR_NEXT(x) (((((x)^((x)>>1)) & 1) << 5) | ((x) >> 1))

#define PGSHIFT 12
#define PGSIZE (1UL << PGSHIFT)

#define SIZEOF_TRAPFRAME_T ((__riscv_xlen / 8) * 36)

#ifndef __ASSEMBLER__

typedef unsigned long pte_t;
#define LEVELS (sizeof(pte_t) == sizeof(uint64_t) ? 3 : 2)
#define PTIDXBITS (PGSHIFT - (sizeof(pte_t) == 8 ? 3 : 2))
#define VPN_BITS (PTIDXBITS * LEVELS)
#define VA_BITS (VPN_BITS + PGSHIFT)
#define PTES_PER_PT (1UL << RISCV_PGLEVEL_BITS)
#define MEGAPAGE_SIZE (PTES_PER_PT * PGSIZE)

typedef struct
{
  long gpr[32];
  long sr;
  long epc;
  long badvaddr;
  long cause;
} trapframe_t;
#endif

#endif