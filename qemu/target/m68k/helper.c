/*
 *  m68k op helpers
 *
 *  Copyright (c) 2006-2007 CodeSourcery
 *  Written by Paul Brook
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#include "qemu/osdep.h"
#include "cpu.h"
#include "exec/exec-all.h"
#include "exec/helper-proto.h"
#include "fpu/softfloat.h"

#define SIGNBIT (1u << 31)

void HELPER(cf_movec_to)(CPUM68KState *env, uint32_t reg, uint32_t val)
{
    switch (reg) {
    case M68K_CR_CACR:
        env->cacr = val;
        m68k_switch_sp(env);
        break;
    case M68K_CR_ACR0:
    case M68K_CR_ACR1:
    case M68K_CR_ACR2:
    case M68K_CR_ACR3:
        /* TODO: Implement Access Control Registers.  */
        break;
    case M68K_CR_VBR:
        env->vbr = val;
        break;
    /* TODO: Implement control registers.  */
    default:
        cpu_abort(env_cpu(env),
                  "Unimplemented control register write 0x%x = 0x%x\n",
                  reg, val);
    }
}

void HELPER(m68k_movec_to)(CPUM68KState *env, uint32_t reg, uint32_t val)
{
    switch (reg) {
    /* MC680[1234]0 */
    case M68K_CR_SFC:
        env->sfc = val & 7;
        return;
    case M68K_CR_DFC:
        env->dfc = val & 7;
        return;
    case M68K_CR_VBR:
        env->vbr = val;
        return;
    /* MC680[2346]0 */
    case M68K_CR_CACR:
        if (m68k_feature(env, M68K_FEATURE_M68020)) {
            env->cacr = val & 0x0000000f;
        } else if (m68k_feature(env, M68K_FEATURE_M68030)) {
            env->cacr = val & 0x00003f1f;
        } else if (m68k_feature(env, M68K_FEATURE_M68040)) {
            env->cacr = val & 0x80008000;
        } else if (m68k_feature(env, M68K_FEATURE_M68060)) {
            env->cacr = val & 0xf8e0e000;
        }
        m68k_switch_sp(env);
        return;
    /* MC680[34]0 */
    case M68K_CR_TC:
        env->mmu.tcr = val;
        return;
    case M68K_CR_MMUSR:
        env->mmu.mmusr = val;
        return;
    case M68K_CR_SRP:
        env->mmu.srp = val;
        return;
    case M68K_CR_URP:
        env->mmu.urp = val;
        return;
    case M68K_CR_USP:
        env->sp[M68K_USP] = val;
        return;
    case M68K_CR_MSP:
        env->sp[M68K_SSP] = val;
        return;
    case M68K_CR_ISP:
        env->sp[M68K_ISP] = val;
        return;
    /* MC68040/MC68LC040 */
    case M68K_CR_ITT0:
        env->mmu.ttr[M68K_ITTR0] = val;
        return;
    case M68K_CR_ITT1:
         env->mmu.ttr[M68K_ITTR1] = val;
        return;
    case M68K_CR_DTT0:
        env->mmu.ttr[M68K_DTTR0] = val;
        return;
    case M68K_CR_DTT1:
        env->mmu.ttr[M68K_DTTR1] = val;
        return;
    }
    cpu_abort(env_cpu(env),
              "Unimplemented control register write 0x%x = 0x%x\n",
              reg, val);
}

uint32_t HELPER(m68k_movec_from)(CPUM68KState *env, uint32_t reg)
{
    switch (reg) {
    /* MC680[1234]0 */
    case M68K_CR_SFC:
        return env->sfc;
    case M68K_CR_DFC:
        return env->dfc;
    case M68K_CR_VBR:
        return env->vbr;
    /* MC680[234]0 */
    case M68K_CR_CACR:
        return env->cacr;
    /* MC680[34]0 */
    case M68K_CR_TC:
        return env->mmu.tcr;
    case M68K_CR_MMUSR:
        return env->mmu.mmusr;
    case M68K_CR_SRP:
        return env->mmu.srp;
    case M68K_CR_USP:
        return env->sp[M68K_USP];
    case M68K_CR_MSP:
        return env->sp[M68K_SSP];
    case M68K_CR_ISP:
        return env->sp[M68K_ISP];
    /* MC68040/MC68LC040 */
    case M68K_CR_URP:
        return env->mmu.urp;
    case M68K_CR_ITT0:
        return env->mmu.ttr[M68K_ITTR0];
    case M68K_CR_ITT1:
        return env->mmu.ttr[M68K_ITTR1];
    case M68K_CR_DTT0:
        return env->mmu.ttr[M68K_DTTR0];
    case M68K_CR_DTT1:
        return env->mmu.ttr[M68K_DTTR1];
    }
    cpu_abort(env_cpu(env), "Unimplemented control register read 0x%x\n",
              reg);
}

void HELPER(set_macsr)(CPUM68KState *env, uint32_t val)
{
    uint32_t acc;
    int8_t exthigh;
    uint8_t extlow;
    uint64_t regval;
    int i;
    if ((env->macsr ^ val) & (MACSR_FI | MACSR_SU)) {
        for (i = 0; i < 4; i++) {
            regval = env->macc[i];
            exthigh = regval >> 40;
            if (env->macsr & MACSR_FI) {
                acc = regval >> 8;
                extlow = regval;
            } else {
                acc = regval;
                extlow = regval >> 32;
            }
            if (env->macsr & MACSR_FI) {
                regval = (((uint64_t)acc) << 8) | extlow;
                regval |= ((int64_t)exthigh) << 40;
            } else if (env->macsr & MACSR_SU) {
                regval = acc | (((int64_t)extlow) << 32);
                regval |= ((int64_t)exthigh) << 40;
            } else {
                regval = acc | (((uint64_t)extlow) << 32);
                regval |= ((uint64_t)(uint8_t)exthigh) << 40;
            }
            env->macc[i] = regval;
        }
    }
    env->macsr = val;
}

void m68k_switch_sp(CPUM68KState *env)
{
    int new_sp;

    env->sp[env->current_sp] = env->aregs[7];
    if (m68k_feature(env, M68K_FEATURE_M68000)) {
        if (env->sr & SR_S) {
            if (env->sr & SR_M) {
                new_sp = M68K_SSP;
            } else {
                new_sp = M68K_ISP;
            }
        } else {
            new_sp = M68K_USP;
        }
    } else {
        new_sp = (env->sr & SR_S && env->cacr & M68K_CACR_EUSP)
                 ? M68K_SSP : M68K_USP;
    }
    env->aregs[7] = env->sp[new_sp];
    env->current_sp = new_sp;
}

static int check_TTR(uint32_t ttr, int *prot, target_ulong addr,
                     int access_type)
{
    uint32_t base, mask;

    /* check if transparent translation is enabled */
    if ((ttr & M68K_TTR_ENABLED) == 0) {
        return 0;
    }

    /* check mode access */
    switch (ttr & M68K_TTR_SFIELD) {
    case M68K_TTR_SFIELD_USER:
        /* match only if user */
        if ((access_type & ACCESS_SUPER) != 0) {
            return 0;
        }
        break;
    case M68K_TTR_SFIELD_SUPER:
        /* match only if supervisor */
        if ((access_type & ACCESS_SUPER) == 0) {
            return 0;
        }
        break;
    default:
        /* all other values disable mode matching (FC2) */
        break;
    }

    /* check address matching */

    base = ttr & M68K_TTR_ADDR_BASE;
    mask = (ttr & M68K_TTR_ADDR_MASK) ^ M68K_TTR_ADDR_MASK;
    mask <<= M68K_TTR_ADDR_MASK_SHIFT;

    if ((addr & mask) != (base & mask)) {
        return 0;
    }

    *prot = PAGE_READ | PAGE_EXEC;
    if ((ttr & M68K_DESC_WRITEPROT) == 0) {
        *prot |= PAGE_WRITE;
    }

    return 1;
}

static int get_physical_address(CPUM68KState *env, hwaddr *physical,
                                int *prot, target_ulong address,
                                int access_type, target_ulong *page_size)
{
    CPUState *cs = env_cpu(env);
    uint32_t entry;
    uint32_t next;
    target_ulong page_mask;
    bool debug = access_type & ACCESS_DEBUG;
    int page_bits;
    int i;
    MemTxResult txres;

    /* Transparent Translation (physical = logical) */
    for (i = 0; i < M68K_MAX_TTR; i++) {
        if (check_TTR(env->mmu.TTR(access_type, i),
                      prot, address, access_type)) {
            if (access_type & ACCESS_PTEST) {
                /* Transparent Translation Register bit */
                env->mmu.mmusr = M68K_MMU_T_040 | M68K_MMU_R_040;
            }
            *physical = address & TARGET_PAGE_MASK;
            *page_size = TARGET_PAGE_SIZE;
            return 0;
        }
    }

    /* Page Table Root Pointer */
    *prot = PAGE_READ | PAGE_WRITE;
    if (access_type & ACCESS_CODE) {
        *prot |= PAGE_EXEC;
    }
    if (access_type & ACCESS_SUPER) {
        next = env->mmu.srp;
    } else {
        next = env->mmu.urp;
    }

    /* Root Index */
    entry = M68K_POINTER_BASE(next) | M68K_ROOT_INDEX(address);

    next = glue(address_space_ldl, UNICORN_ARCH_POSTFIX)(cs->as->uc, cs->as, entry, MEMTXATTRS_UNSPECIFIED, &txres);
    if (txres != MEMTX_OK) {
        goto txfail;
    }
    if (!M68K_UDT_VALID(next)) {
        return -1;
    }
    if (!(next & M68K_DESC_USED) && !debug) {
        glue(address_space_stl, UNICORN_ARCH_POSTFIX)(cs->as->uc, cs->as, entry, next | M68K_DESC_USED,
                          MEMTXATTRS_UNSPECIFIED, &txres);
        if (txres != MEMTX_OK) {
            goto txfail;
        }
    }
    if (next & M68K_DESC_WRITEPROT) {
        if (access_type & ACCESS_PTEST) {
            env->mmu.mmusr |= M68K_MMU_WP_040;
        }
        *prot &= ~PAGE_WRITE;
        if (access_type & ACCESS_STORE) {
            return -1;
        }
    }

    /* Pointer Index */
    entry = M68K_POINTER_BASE(next) | M68K_POINTER_INDEX(address);

    next = glue(address_space_ldl, UNICORN_ARCH_POSTFIX)(cs->as->uc, cs->as, entry, MEMTXATTRS_UNSPECIFIED, &txres);
    if (txres != MEMTX_OK) {
        goto txfail;
    }
    if (!M68K_UDT_VALID(next)) {
        return -1;
    }
    if (!(next & M68K_DESC_USED) && !debug) {
        glue(address_space_stl, UNICORN_ARCH_POSTFIX)(cs->as->uc, cs->as, entry, next | M68K_DESC_USED,
                          MEMTXATTRS_UNSPECIFIED, &txres);
        if (txres != MEMTX_OK) {
            goto txfail;
        }
    }
    if (next & M68K_DESC_WRITEPROT) {
        if (access_type & ACCESS_PTEST) {
            env->mmu.mmusr |= M68K_MMU_WP_040;
        }
        *prot &= ~PAGE_WRITE;
        if (access_type & ACCESS_STORE) {
            return -1;
        }
    }

    /* Page Index */
    if (env->mmu.tcr & M68K_TCR_PAGE_8K) {
        entry = M68K_8K_PAGE_BASE(next) | M68K_8K_PAGE_INDEX(address);
    } else {
        entry = M68K_4K_PAGE_BASE(next) | M68K_4K_PAGE_INDEX(address);
    }

    next = glue(address_space_ldl, UNICORN_ARCH_POSTFIX)(cs->as->uc, cs->as, entry, MEMTXATTRS_UNSPECIFIED, &txres);
    if (txres != MEMTX_OK) {
        goto txfail;
    }

    if (!M68K_PDT_VALID(next)) {
        return -1;
    }
    if (M68K_PDT_INDIRECT(next)) {
        next = glue(address_space_ldl, UNICORN_ARCH_POSTFIX)(cs->as->uc, cs->as, M68K_INDIRECT_POINTER(next),
                                 MEMTXATTRS_UNSPECIFIED, &txres);
        if (txres != MEMTX_OK) {
            goto txfail;
        }
    }
    if (access_type & ACCESS_STORE) {
        if (next & M68K_DESC_WRITEPROT) {
            if (!(next & M68K_DESC_USED) && !debug) {
                glue(address_space_stl, UNICORN_ARCH_POSTFIX)(cs->as->uc, cs->as, entry, next | M68K_DESC_USED,
                                  MEMTXATTRS_UNSPECIFIED, &txres);
                if (txres != MEMTX_OK) {
                    goto txfail;
                }
            }
        } else if ((next & (M68K_DESC_MODIFIED | M68K_DESC_USED)) !=
                           (M68K_DESC_MODIFIED | M68K_DESC_USED) && !debug) {
            glue(address_space_stl, UNICORN_ARCH_POSTFIX)(cs->as->uc, cs->as, entry,
                              next | (M68K_DESC_MODIFIED | M68K_DESC_USED),
                              MEMTXATTRS_UNSPECIFIED, &txres);
            if (txres != MEMTX_OK) {
                goto txfail;
            }
        }
    } else {
        if (!(next & M68K_DESC_USED) && !debug) {
            glue(address_space_stl, UNICORN_ARCH_POSTFIX)(cs->as->uc, cs->as, entry, next | M68K_DESC_USED,
                              MEMTXATTRS_UNSPECIFIED, &txres);
            if (txres != MEMTX_OK) {
                goto txfail;
            }
        }
    }

    if (env->mmu.tcr & M68K_TCR_PAGE_8K) {
        page_bits = 13;
    } else {
        page_bits = 12;
    }
    *page_size = 1 << page_bits;
    page_mask = ~(*page_size - 1);
    *physical = next & page_mask;

    if (access_type & ACCESS_PTEST) {
        env->mmu.mmusr |= next & M68K_MMU_SR_MASK_040;
        env->mmu.mmusr |= *physical & 0xfffff000;
        env->mmu.mmusr |= M68K_MMU_R_040;
    }

    if (next & M68K_DESC_WRITEPROT) {
        *prot &= ~PAGE_WRITE;
        if (access_type & ACCESS_STORE) {
            return -1;
        }
    }
    if (next & M68K_DESC_SUPERONLY) {
        if ((access_type & ACCESS_SUPER) == 0) {
            return -1;
        }
    }

    return 0;

txfail:
    /*
     * A page table load/store failed. TODO: we should really raise a
     * suitable guest fault here if this is not a debug access.
     * For now just return that the translation failed.
     */
    return -1;
}

hwaddr m68k_cpu_get_phys_page_debug(CPUState *cs, vaddr addr)
{
    M68kCPU *cpu = M68K_CPU(cs);
    CPUM68KState *env = &cpu->env;
    hwaddr phys_addr;
    int prot;
    int access_type;
    target_ulong page_size;

    if ((env->mmu.tcr & M68K_TCR_ENABLED) == 0) {
        /* MMU disabled */
        return addr;
    }

    access_type = ACCESS_DATA | ACCESS_DEBUG;
    if (env->sr & SR_S) {
        access_type |= ACCESS_SUPER;
    }
    if (get_physical_address(env, &phys_addr, &prot,
                             addr, access_type, &page_size) != 0) {
        return -1;
    }
    return phys_addr;
}

/*
 * Notify CPU of a pending interrupt.  Prioritization and vectoring should
 * be handled by the interrupt controller.  Real hardware only requests
 * the vector when the interrupt is acknowledged by the CPU.  For
 * simplicity we calculate it when the interrupt is signalled.
 */
void m68k_set_irq_level(M68kCPU *cpu, int level, uint8_t vector)
{
    CPUState *cs = CPU(cpu);
    CPUM68KState *env = &cpu->env;

    env->pending_level = level;
    env->pending_vector = vector;
    if (level) {
        cpu_interrupt(cs, CPU_INTERRUPT_HARD);
    } else {
        cpu_reset_interrupt(cs, CPU_INTERRUPT_HARD);
    }
}

bool m68k_cpu_tlb_fill(CPUState *cs, vaddr address, int size,
                       MMUAccessType qemu_access_type, int mmu_idx,
                       bool probe, uintptr_t retaddr)
{
    M68kCPU *cpu = M68K_CPU(cs);
    CPUM68KState *env = &cpu->env;

    hwaddr physical;
    int prot;
    int access_type;
    int ret;
    target_ulong page_size;

    if ((env->mmu.tcr & M68K_TCR_ENABLED) == 0) {
        /* MMU disabled */
        tlb_set_page(cs, address & TARGET_PAGE_MASK,
                     address & TARGET_PAGE_MASK,
                     PAGE_READ | PAGE_WRITE | PAGE_EXEC,
                     mmu_idx, TARGET_PAGE_SIZE);
        return true;
    }

    if (qemu_access_type == MMU_INST_FETCH) {
        access_type = ACCESS_CODE;
    } else {
        access_type = ACCESS_DATA;
        if (qemu_access_type == MMU_DATA_STORE) {
            access_type |= ACCESS_STORE;
        }
    }
    if (mmu_idx != MMU_USER_IDX) {
        access_type |= ACCESS_SUPER;
    }

    ret = get_physical_address(&cpu->env, &physical, &prot,
                               address, access_type, &page_size);
    if (likely(ret == 0)) {
        address &= TARGET_PAGE_MASK;
        physical += address & (page_size - 1);
        tlb_set_page(cs, address, physical,
                     prot, mmu_idx, TARGET_PAGE_SIZE);
        return true;
    }

    if (probe) {
        return false;
    }

    /* page fault */
    env->mmu.ssw = M68K_ATC_040;
    switch (size) {
    case 1:
        env->mmu.ssw |= M68K_BA_SIZE_BYTE;
        break;
    case 2:
        env->mmu.ssw |= M68K_BA_SIZE_WORD;
        break;
    case 4:
        env->mmu.ssw |= M68K_BA_SIZE_LONG;
        break;
    }
    if (access_type & ACCESS_SUPER) {
        env->mmu.ssw |= M68K_TM_040_SUPER;
    }
    if (access_type & ACCESS_CODE) {
        env->mmu.ssw |= M68K_TM_040_CODE;
    } else {
        env->mmu.ssw |= M68K_TM_040_DATA;
    }
    if (!(access_type & ACCESS_STORE)) {
        env->mmu.ssw |= M68K_RW_040;
    }

    cs->exception_index = EXCP_ACCESS;
    env->mmu.ar = address;
    cpu_loop_exit_restore(cs, retaddr);
}

uint32_t HELPER(bitrev)(uint32_t x)
{
    x = ((x >> 1) & 0x55555555u) | ((x << 1) & 0xaaaaaaaau);
    x = ((x >> 2) & 0x33333333u) | ((x << 2) & 0xccccccccu);
    x = ((x >> 4) & 0x0f0f0f0fu) | ((x << 4) & 0xf0f0f0f0u);
    return bswap32(x);
}

uint32_t HELPER(ff1)(uint32_t x)
{
    int n;
    for (n = 32; x; n--)
        x >>= 1;
    return n;
}

uint32_t HELPER(sats)(uint32_t val, uint32_t v)
{
    /* The result has the opposite sign to the original value.  */
    if ((int32_t)v < 0) {
        val = (((int32_t)val) >> 31) ^ SIGNBIT;
    }
    return val;
}

uint32_t cpu_m68k_get_sr(CPUM68KState *env) {
    return env->sr | cpu_m68k_get_ccr(env);
}

void cpu_m68k_set_sr(CPUM68KState *env, uint32_t sr)
{
    env->sr = sr & 0xffe0;
    cpu_m68k_set_ccr(env, sr);
    m68k_switch_sp(env);
}

void HELPER(set_sr)(CPUM68KState *env, uint32_t val)
{
    cpu_m68k_set_sr(env, val);
}

/* MAC unit.  */
/*
 * FIXME: The MAC unit implementation is a bit of a mess.  Some helpers
 * take values,  others take register numbers and manipulate the contents
 * in-place.
 */
void HELPER(mac_move)(CPUM68KState *env, uint32_t dest, uint32_t src)
{
    uint32_t mask;
    env->macc[dest] = env->macc[src];
    mask = MACSR_PAV0 << dest;
    if (env->macsr & (MACSR_PAV0 << src))
        env->macsr |= mask;
    else
        env->macsr &= ~mask;
}

uint64_t HELPER(macmuls)(CPUM68KState *env, uint32_t op1, uint32_t op2)
{
    int64_t product;
    int64_t res;

    product = (uint64_t)op1 * op2;
    res = (product << 24) >> 24;
    if (res != product) {
        env->macsr |= MACSR_V;
        if (env->macsr & MACSR_OMC) {
            /* Make sure the accumulate operation overflows.  */
            if (product < 0)
                res = ~(1ll << 50);
            else
                res = 1ll << 50;
        }
    }
    return res;
}

uint64_t HELPER(macmulu)(CPUM68KState *env, uint32_t op1, uint32_t op2)
{
    uint64_t product;

    product = (uint64_t)op1 * op2;
    if (product & (0xffffffull << 40)) {
        env->macsr |= MACSR_V;
        if (env->macsr & MACSR_OMC) {
            /* Make sure the accumulate operation overflows.  */
            product = 1ll << 50;
        } else {
            product &= ((1ull << 40) - 1);
        }
    }
    return product;
}

uint64_t HELPER(macmulf)(CPUM68KState *env, uint32_t op1, uint32_t op2)
{
    uint64_t product;
    uint32_t remainder;

    product = (uint64_t)op1 * op2;
    if (env->macsr & MACSR_RT) {
        remainder = product & 0xffffff;
        product >>= 24;
        if (remainder > 0x800000)
            product++;
        else if (remainder == 0x800000)
            product += (product & 1);
    } else {
        product >>= 24;
    }
    return product;
}

void HELPER(macsats)(CPUM68KState *env, uint32_t acc)
{
    int64_t tmp;
    int64_t result;
    tmp = env->macc[acc];
    result = ((tmp << 16) >> 16);
    if (result != tmp) {
        env->macsr |= MACSR_V;
    }
    if (env->macsr & MACSR_V) {
        env->macsr |= MACSR_PAV0 << acc;
        if (env->macsr & MACSR_OMC) {
            /*
             * The result is saturated to 32 bits, despite overflow occurring
             * at 48 bits.  Seems weird, but that's what the hardware docs
             * say.
             */
            result = (result >> 63) ^ 0x7fffffff;
        }
    }
    env->macc[acc] = result;
}

void HELPER(macsatu)(CPUM68KState *env, uint32_t acc)
{
    uint64_t val;

    val = env->macc[acc];
    if (val & (0xffffull << 48)) {
        env->macsr |= MACSR_V;
    }
    if (env->macsr & MACSR_V) {
        env->macsr |= MACSR_PAV0 << acc;
        if (env->macsr & MACSR_OMC) {
            if (val > (1ull << 53))
                val = 0;
            else
                val = (1ull << 48) - 1;
        } else {
            val &= ((1ull << 48) - 1);
        }
    }
    env->macc[acc] = val;
}

void HELPER(macsatf)(CPUM68KState *env, uint32_t acc)
{
    int64_t sum;
    int64_t result;

    sum = env->macc[acc];
    result = (sum << 16) >> 16;
    if (result != sum) {
        env->macsr |= MACSR_V;
    }
    if (env->macsr & MACSR_V) {
        env->macsr |= MACSR_PAV0 << acc;
        if (env->macsr & MACSR_OMC) {
            result = (result >> 63) ^ 0x7fffffffffffll;
        }
    }
    env->macc[acc] = result;
}

void HELPER(mac_set_flags)(CPUM68KState *env, uint32_t acc)
{
    uint64_t val;
    val = env->macc[acc];
    if (val == 0) {
        env->macsr |= MACSR_Z;
    } else if (val & (1ull << 47)) {
        env->macsr |= MACSR_N;
    }
    if (env->macsr & (MACSR_PAV0 << acc)) {
        env->macsr |= MACSR_V;
    }
    if (env->macsr & MACSR_FI) {
        val = ((int64_t)val) >> 40;
        if (val != 0 && val != -1)
            env->macsr |= MACSR_EV;
    } else if (env->macsr & MACSR_SU) {
        val = ((int64_t)val) >> 32;
        if (val != 0 && val != -1)
            env->macsr |= MACSR_EV;
    } else {
        if ((val >> 32) != 0)
            env->macsr |= MACSR_EV;
    }
}

#define EXTSIGN(val, index) (     \
    (index == 0) ? (int8_t)(val) : ((index == 1) ? (int16_t)(val) : (val)) \
)

#define COMPUTE_CCR(op, x, n, z, v, c) {                                   \
    switch (op) {                                                          \
    case CC_OP_FLAGS:                                                      \
        /* Everything in place.  */                                        \
        break;                                                             \
    case CC_OP_ADDB:                                                       \
    case CC_OP_ADDW:                                                       \
    case CC_OP_ADDL:                                                       \
        res = n;                                                           \
        src2 = v;                                                          \
        src1 = EXTSIGN(res - src2, op - CC_OP_ADDB);                       \
        c = x;                                                             \
        z = n;                                                             \
        v = (res ^ src1) & ~(src1 ^ src2);                                 \
        break;                                                             \
    case CC_OP_SUBB:                                                       \
    case CC_OP_SUBW:                                                       \
    case CC_OP_SUBL:                                                       \
        res = n;                                                           \
        src2 = v;                                                          \
        src1 = EXTSIGN(res + src2, op - CC_OP_SUBB);                       \
        c = x;                                                             \
        z = n;                                                             \
        v = (res ^ src1) & (src1 ^ src2);                                  \
        break;                                                             \
    case CC_OP_CMPB:                                                       \
    case CC_OP_CMPW:                                                       \
    case CC_OP_CMPL:                                                       \
        src1 = n;                                                          \
        src2 = v;                                                          \
        res = EXTSIGN(src1 - src2, op - CC_OP_CMPB);                       \
        n = res;                                                           \
        z = res;                                                           \
        c = src1 < src2;                                                   \
        v = (res ^ src1) & (src1 ^ src2);                                  \
        break;                                                             \
    case CC_OP_LOGIC:                                                      \
        c = v = 0;                                                         \
        z = n;                                                             \
        break;                                                             \
    default:                                                               \
        cpu_abort(env_cpu(env), "Bad CC_OP %d", op);                       \
    }                                                                      \
} while (0)

uint32_t cpu_m68k_get_ccr(CPUM68KState *env)
{
    uint32_t x, c, n, z, v;
    uint32_t res, src1, src2;

    x = env->cc_x;
    n = env->cc_n;
    z = env->cc_z;
    v = env->cc_v;
    c = env->cc_c;

    COMPUTE_CCR(env->cc_op, x, n, z, v, c);

    n = n >> 31;
    z = (z == 0);
    v = v >> 31;

    return x * CCF_X + n * CCF_N + z * CCF_Z + v * CCF_V + c * CCF_C;
}

uint32_t HELPER(get_ccr)(CPUM68KState *env)
{
    return cpu_m68k_get_ccr(env);
}

void cpu_m68k_set_ccr(CPUM68KState *env, uint32_t ccr)
{
    env->cc_x = (ccr & CCF_X ? 1 : 0);
    env->cc_n = (ccr & CCF_N ? -1 : 0);
    env->cc_z = (ccr & CCF_Z ? 0 : 1);
    env->cc_v = (ccr & CCF_V ? -1 : 0);
    env->cc_c = (ccr & CCF_C ? 1 : 0);
    env->cc_op = CC_OP_FLAGS;
}

void HELPER(set_ccr)(CPUM68KState *env, uint32_t ccr)
{
    cpu_m68k_set_ccr(env, ccr);
}

void HELPER(flush_flags)(CPUM68KState *env, uint32_t cc_op)
{
    uint32_t res, src1, src2;

    COMPUTE_CCR(cc_op, env->cc_x, env->cc_n, env->cc_z, env->cc_v, env->cc_c);
    env->cc_op = CC_OP_FLAGS;
}

uint32_t HELPER(get_macf)(CPUM68KState *env, uint64_t val)
{
    int rem;
    uint32_t result;

    if (env->macsr & MACSR_SU) {
        /* 16-bit rounding.  */
        rem = val & 0xffffff;
        val = (val >> 24) & 0xffffu;
        if (rem > 0x800000)
            val++;
        else if (rem == 0x800000)
            val += (val & 1);
    } else if (env->macsr & MACSR_RT) {
        /* 32-bit rounding.  */
        rem = val & 0xff;
        val >>= 8;
        if (rem > 0x80)
            val++;
        else if (rem == 0x80)
            val += (val & 1);
    } else {
        /* No rounding.  */
        val >>= 8;
    }
    if (env->macsr & MACSR_OMC) {
        /* Saturate.  */
        if (env->macsr & MACSR_SU) {
            if (val != (uint16_t) val) {
                result = ((val >> 63) ^ 0x7fff) & 0xffff;
            } else {
                result = val & 0xffff;
            }
        } else {
            if (val != (uint32_t)val) {
                result = ((uint32_t)(val >> 63) & 0x7fffffff);
            } else {
                result = (uint32_t)val;
            }
        }
    } else {
        /* No saturation.  */
        if (env->macsr & MACSR_SU) {
            result = val & 0xffff;
        } else {
            result = (uint32_t)val;
        }
    }
    return result;
}

uint32_t HELPER(get_macs)(uint64_t val)
{
    if (val == (int32_t)val) {
        return (int32_t)val;
    } else {
        return (val >> 61) ^ ~SIGNBIT;
    }
}

uint32_t HELPER(get_macu)(uint64_t val)
{
    if ((val >> 32) == 0) {
        return (uint32_t)val;
    } else {
        return 0xffffffffu;
    }
}

uint32_t HELPER(get_mac_extf)(CPUM68KState *env, uint32_t acc)
{
    uint32_t val;
    val = env->macc[acc] & 0x00ff;
    val |= (env->macc[acc] >> 32) & 0xff00;
    val |= (env->macc[acc + 1] << 16) & 0x00ff0000;
    val |= (env->macc[acc + 1] >> 16) & 0xff000000;
    return val;
}

uint32_t HELPER(get_mac_exti)(CPUM68KState *env, uint32_t acc)
{
    uint32_t val;
    val = (env->macc[acc] >> 32) & 0xffff;
    val |= (env->macc[acc + 1] >> 16) & 0xffff0000;
    return val;
}

void HELPER(set_mac_extf)(CPUM68KState *env, uint32_t val, uint32_t acc)
{
    int64_t res;
    int32_t tmp;
    res = env->macc[acc] & 0xffffffff00ull;
    tmp = (int16_t)(val & 0xff00);
    res |= ((int64_t)tmp) << 32;
    res |= val & 0xff;
    env->macc[acc] = res;
    res = env->macc[acc + 1] & 0xffffffff00ull;
    tmp = (val & 0xff000000);
    res |= ((int64_t)tmp) << 16;
    res |= (val >> 16) & 0xff;
    env->macc[acc + 1] = res;
}

void HELPER(set_mac_exts)(CPUM68KState *env, uint32_t val, uint32_t acc)
{
    int64_t res;
    int32_t tmp;
    res = (uint32_t)env->macc[acc];
    tmp = (int16_t)val;
    res |= ((int64_t)tmp) << 32;
    env->macc[acc] = res;
    res = (uint32_t)env->macc[acc + 1];
    tmp = val & 0xffff0000;
    res |= (int64_t)tmp << 16;
    env->macc[acc + 1] = res;
}

void HELPER(set_mac_extu)(CPUM68KState *env, uint32_t val, uint32_t acc)
{
    uint64_t res;
    res = (uint32_t)env->macc[acc];
    res |= ((uint64_t)(val & 0xffff)) << 32;
    env->macc[acc] = res;
    res = (uint32_t)env->macc[acc + 1];
    res |= (uint64_t)(val & 0xffff0000) << 16;
    env->macc[acc + 1] = res;
}

void HELPER(ptest)(CPUM68KState *env, uint32_t addr, uint32_t is_read)
{
    hwaddr physical;
    int access_type;
    int prot;
    int ret;
    target_ulong page_size;

    access_type = ACCESS_PTEST;
    if (env->dfc & 4) {
        access_type |= ACCESS_SUPER;
    }
    if ((env->dfc & 3) == 2) {
        access_type |= ACCESS_CODE;
    }
    if (!is_read) {
        access_type |= ACCESS_STORE;
    }

    env->mmu.mmusr = 0;
    env->mmu.ssw = 0;
    ret = get_physical_address(env, &physical, &prot, addr,
                               access_type, &page_size);
    if (ret == 0) {
        addr &= TARGET_PAGE_MASK;
        physical += addr & (page_size - 1);
        tlb_set_page(env_cpu(env), addr, physical,
                     prot, access_type & ACCESS_SUPER ?
                     MMU_KERNEL_IDX : MMU_USER_IDX, page_size);
    }
}

void HELPER(pflush)(CPUM68KState *env, uint32_t addr, uint32_t opmode)
{
    CPUState *cs = env_cpu(env);

    switch (opmode) {
    case 0: /* Flush page entry if not global */
    case 1: /* Flush page entry */
        tlb_flush_page(cs, addr);
        break;
    case 2: /* Flush all except global entries */
        tlb_flush(cs);
        break;
    case 3: /* Flush all entries */
        tlb_flush(cs);
        break;
    }
}

void HELPER(reset)(CPUM68KState *env)
{
    /* FIXME: reset all except CPU */
}
