#include "unicorn_test.h"

static void test_map_correct(void)
{
    uc_engine *uc;

    OK(uc_open(UC_ARCH_X86, UC_MODE_64, &uc));
    OK(uc_mem_map(uc, 0x40000, 0x1000 * 16, UC_PROT_ALL)); // [0x40000, 0x50000]
    OK(uc_mem_map(uc, 0x60000, 0x1000 * 16, UC_PROT_ALL)); // [0x60000, 0x70000]
    OK(uc_mem_map(uc, 0x20000, 0x1000 * 16, UC_PROT_ALL)); // [0x20000, 0x30000]
    uc_assert_err(UC_ERR_MAP,
                  uc_mem_map(uc, 0x10000, 0x2000 * 16, UC_PROT_ALL));
    uc_assert_err(UC_ERR_MAP,
                  uc_mem_map(uc, 0x25000, 0x1000 * 16, UC_PROT_ALL));
    uc_assert_err(UC_ERR_MAP,
                  uc_mem_map(uc, 0x35000, 0x1000 * 16, UC_PROT_ALL));
    uc_assert_err(UC_ERR_MAP,
                  uc_mem_map(uc, 0x45000, 0x1000 * 16, UC_PROT_ALL));
    uc_assert_err(UC_ERR_MAP,
                  uc_mem_map(uc, 0x55000, 0x2000 * 16, UC_PROT_ALL));
    OK(uc_mem_map(uc, 0x35000, 0x5000, UC_PROT_ALL));
    OK(uc_mem_map(uc, 0x50000, 0x5000, UC_PROT_ALL));

    OK(uc_close(uc));
}

static void test_map_wrapping(void)
{
    uc_engine *uc;

    OK(uc_open(UC_ARCH_X86, UC_MODE_64, &uc));
    uc_assert_err(UC_ERR_ARG, uc_mem_map(uc, (~0ll - 0x4000) & ~0xfff, 0x8000,
                                         UC_PROT_ALL));

    OK(uc_close(uc));
}

static void test_mem_protect(void)
{
    uc_engine *qc;
    int r_eax = 0x2000;
    int r_esi = 0xdeadbeef;
    uint32_t mem;
    // add [eax + 4], esi
    char code[] = {0x01, 0x70, 0x04};

    OK(uc_open(UC_ARCH_X86, UC_MODE_32, &qc));
    OK(uc_reg_write(qc, UC_X86_REG_EAX, &r_eax));
    OK(uc_reg_write(qc, UC_X86_REG_ESI, &r_esi));
    OK(uc_mem_map(qc, 0x1000, 0x1000, UC_PROT_READ | UC_PROT_EXEC));
    OK(uc_mem_map(qc, 0x2000, 0x1000, UC_PROT_READ));
    OK(uc_mem_protect(qc, 0x2000, 0x1000, UC_PROT_READ | UC_PROT_WRITE));
    OK(uc_mem_write(qc, 0x1000, code, sizeof(code)));

    OK(uc_emu_start(qc, 0x1000, 0x1000 + sizeof(code) - 1, 0, 1));
    OK(uc_mem_read(qc, 0x2000 + 4, &mem, 4));

    TEST_CHECK(LEINT32(mem) == 0xdeadbeef);

    OK(uc_close(qc));
}

static void test_splitting_mem_unmap(void)
{
    uc_engine *uc;

    OK(uc_open(UC_ARCH_X86, UC_MODE_32, &uc));

    OK(uc_mem_map(uc, 0x20000, 0x1000, UC_PROT_NONE));
    OK(uc_mem_map(uc, 0x21000, 0x2000, UC_PROT_NONE));

    OK(uc_mem_unmap(uc, 0x21000, 0x1000));

    OK(uc_close(uc));
}

static uint64_t test_splitting_mmio_unmap_read_callback(uc_engine *uc,
                                                        uint64_t offset,
                                                        unsigned size,
                                                        void *user_data)
{
    TEST_CHECK(offset == 4);
    TEST_CHECK(size == 4);

    return 0x19260817;
}

static void test_splitting_mmio_unmap(void)
{
    uc_engine *uc;
    // mov ecx, [0x3004] <-- normal read
    // mov ebx, [0x4004] <-- mmio read
    char code[] = "\x8b\x0d\x04\x30\x00\x00\x8b\x1d\x04\x40\x00\x00";
    int r_ecx, r_ebx;
    int bytes = LEINT32(0xdeadbeef);

    OK(uc_open(UC_ARCH_X86, UC_MODE_32, &uc));

    OK(uc_mem_map(uc, 0x1000, 0x1000, UC_PROT_ALL));
    OK(uc_mem_write(uc, 0x1000, code, sizeof(code) - 1));

    OK(uc_mmio_map(uc, 0x3000, 0x2000, test_splitting_mmio_unmap_read_callback,
                   NULL, NULL, NULL));

    // Map a ram area instead
    OK(uc_mem_unmap(uc, 0x3000, 0x1000));
    OK(uc_mem_map(uc, 0x3000, 0x1000, UC_PROT_ALL));
    OK(uc_mem_write(uc, 0x3004, &bytes, 4));

    OK(uc_emu_start(uc, 0x1000, 0x1000 + sizeof(code) - 1, 0, 0));

    OK(uc_reg_read(uc, UC_X86_REG_ECX, &r_ecx));
    OK(uc_reg_read(uc, UC_X86_REG_EBX, &r_ebx));

    TEST_CHECK(r_ecx == 0xdeadbeef);
    TEST_CHECK(r_ebx == 0x19260817);

    OK(uc_close(uc));
}

static void test_mem_protect_map_ptr(void)
{
    uc_engine *uc;
    uint64_t val = 0x114514;
    uint8_t *data1 = NULL;
    uint8_t *data2 = NULL;
    uint64_t mem;

    data1 = calloc(sizeof(*data1), 0x4000);
    data2 = calloc(sizeof(*data2), 0x2000);

    OK(uc_open(UC_ARCH_X86, UC_MODE_64, &uc));

    OK(uc_mem_map_ptr(uc, 0x4000, 0x4000, UC_PROT_ALL, data1));
    OK(uc_mem_unmap(uc, 0x6000, 0x2000));
    OK(uc_mem_map_ptr(uc, 0x6000, 0x2000, UC_PROT_ALL, data2));

    OK(uc_mem_write(uc, 0x6004, &val, 8));
    OK(uc_mem_protect(uc, 0x6000, 0x1000, UC_PROT_READ));
    OK(uc_mem_read(uc, 0x6004, (void *)&mem, 8));

    TEST_CHECK(val == mem);

    OK(uc_close(uc));

    free(data2);
    free(data1);
}

static void test_map_at_the_end(void)
{
    uc_engine *uc;
    uint8_t mem[0x1000];

    memset(mem, 0xff, 0x100);

    OK(uc_open(UC_ARCH_X86, UC_MODE_64, &uc));

    OK(uc_mem_map(uc, 0xfffffffffffff000, 0x1000, UC_PROT_ALL));
    OK(uc_mem_write(uc, 0xfffffffffffff000, mem, sizeof(mem)));

    uc_assert_err(UC_ERR_WRITE_UNMAPPED,
                  uc_mem_write(uc, 0xffffffffffffff00, mem, sizeof(mem)));
    uc_assert_err(UC_ERR_WRITE_UNMAPPED, uc_mem_write(uc, 0, mem, sizeof(mem)));

    OK(uc_close(uc));
}

static void test_map_wrap(void)
{
    uc_engine *uc;

    OK(uc_open(UC_ARCH_X86, UC_MODE_64, &uc));

    uc_assert_err(UC_ERR_ARG,
                  uc_mem_map(uc, 0xfffffffffffff000, 0x2000, UC_PROT_ALL));

    OK(uc_close(uc));
}

static void test_map_big_memory(void)
{
    uc_engine *uc;

    OK(uc_open(UC_ARCH_X86, UC_MODE_64, &uc));

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
    uint64_t requested_size = 0xfffffffffffff000; // assume 4K page size
#else
    long ps = sysconf(_SC_PAGESIZE);
    uint64_t requested_size = (uint64_t)(-ps);
#endif

    uc_assert_err(UC_ERR_NOMEM,
                  uc_mem_map(uc, 0x0, requested_size, UC_PROT_ALL));

    OK(uc_close(uc));
}

static void test_mem_protect_remove_exec_callback(uc_engine *uc, uint64_t addr,
                                                  size_t size, void *data)
{
    uint64_t *p = (uint64_t *)data;
    (*p)++;

    OK(uc_mem_protect(uc, 0x2000, 0x1000, UC_PROT_READ));
}

static void test_mem_protect_remove_exec(void)
{
    uc_engine *uc;
    char code[] = "\x90\xeb\x00\x90";
    uc_hook hk;
    uint64_t called_count = 0;

    OK(uc_open(UC_ARCH_X86, UC_MODE_64, &uc));
    OK(uc_mem_map(uc, 0x1000, 0x1000, UC_PROT_ALL));
    OK(uc_mem_map(uc, 0x2000, 0x1000, UC_PROT_ALL));

    OK(uc_mem_write(uc, 0x1000, code, sizeof(code) - 1));
    OK(uc_hook_add(uc, &hk, UC_HOOK_BLOCK,
                   test_mem_protect_remove_exec_callback, (void *)&called_count,
                   1, 0));

    OK(uc_emu_start(uc, 0x1000, 0x1000 + sizeof(code) - 1, 0, 0));

    TEST_CHECK(called_count == 2);

    OK(uc_close(uc));
}

static uint64_t test_mem_protect_mmio_read_cb(struct uc_struct *uc,
                                              uint64_t addr, unsigned size,
                                              void *user_data)
{
    TEST_CHECK(addr == 0x20); // note, it's not 0x1020

    *(uint64_t *)user_data += 1;
    return 0x114514;
}

static void test_mem_protect_mmio_write_cb(struct uc_struct *uc, uint64_t addr,
                                           unsigned size, uint64_t data,
                                           void *user_data)
{
    TEST_CHECK(false);
    return;
}

static void test_mem_protect_mmio(void)
{
    uc_engine *uc;
    // mov eax, [0x2020]; mov [0x2020], eax
    char code[] = "\xa1\x20\x20\x00\x00\x00\x00\x00\x00\xa3\x20\x20\x00\x00\x00"
                  "\x00\x00\x00";
    uint64_t called = 0;
    uint64_t r_eax;

    OK(uc_open(UC_ARCH_X86, UC_MODE_64, &uc));
    OK(uc_mem_map(uc, 0x8000, 0x1000, UC_PROT_ALL));
    OK(uc_mem_write(uc, 0x8000, code, sizeof(code) - 1));

    OK(uc_mmio_map(uc, 0x1000, 0x3000, test_mem_protect_mmio_read_cb,
                   (void *)&called, test_mem_protect_mmio_write_cb,
                   (void *)&called));
    OK(uc_mem_protect(uc, 0x2000, 0x1000, UC_PROT_READ));

    uc_assert_err(UC_ERR_WRITE_PROT,
                  uc_emu_start(uc, 0x8000, 0x8000 + sizeof(code) - 1, 0, 0));
    OK(uc_reg_read(uc, UC_X86_REG_RAX, &r_eax));

    TEST_CHECK(called == 1);
    TEST_CHECK(r_eax == 0x114514);

    OK(uc_close(uc));
}

static void test_snapshot(void)
{
    uc_engine *uc;
    uc_context *c0, *c1;
    uint32_t mem;
    uint8_t code_data;
    // mov eax, [0x2020]; inc eax; mov [0x2020], eax
    char code[] = "\xa1\x20\x20\x00\x00\x00\x00\x00\x00\xff\xc0\xa3\x20\x20\x00"
                  "\x00\x00\x00\x00\x00";

    OK(uc_open(UC_ARCH_X86, UC_MODE_64, &uc));
    OK(uc_context_alloc(uc, &c0));
    OK(uc_context_alloc(uc, &c1));
    OK(uc_ctl_context_mode(uc, UC_CTL_CONTEXT_MEMORY));
    OK(uc_mem_map(uc, 0x1000, 0x1000, UC_PROT_ALL));
    OK(uc_mem_write(uc, 0x1000, code, sizeof(code) - 1));

    OK(uc_mem_map(uc, 0x2000, 0x1000, UC_PROT_ALL));
    OK(uc_context_save(uc, c0));

    OK(uc_emu_start(uc, 0x1000, 0x1000 + sizeof(code) - 1, 0, 0));
    OK(uc_mem_read(uc, 0x2020, &mem, sizeof(mem)));
    TEST_CHECK(LEINT32(mem) == 1);
    OK(uc_context_save(uc, c1));
    OK(uc_emu_start(uc, 0x1000, 0x1000 + sizeof(code) - 1, 0, 0));
    OK(uc_mem_read(uc, 0x2020, &mem, sizeof(mem)));
    TEST_CHECK(LEINT32(mem) == 2);
    OK(uc_context_restore(uc, c1));

    OK(uc_mem_read(uc, 0x2020, &mem, sizeof(mem)));
    TEST_CHECK(LEINT32(mem) == 1);
    OK(uc_context_restore(uc, c0));
    OK(uc_mem_read(uc, 0x2020, &mem, sizeof(mem)));
    TEST_CHECK(LEINT32(mem) == 0);

    OK(uc_mem_read(uc, 0x1000, &code_data, sizeof(code_data)));
    TEST_CHECK(code_data == 0xa1);

    OK(uc_context_free(c0));
    OK(uc_context_free(c1));
    OK(uc_close(uc));
}

static bool test_snapshot_with_vtlb_callback(uc_engine *uc, uint64_t addr,
                                             uc_mem_type type,
                                             uc_tlb_entry *result,
                                             void *user_data)
{
    result->paddr = addr - 0x400000000;
    result->perms = UC_PROT_ALL;
    return true;
}

static void test_snapshot_with_vtlb(void)
{
    uc_engine *uc;
    uc_context *c0, *c1;
    uint32_t mem;
    uc_hook hook;

    // mov eax, [0x2020]; inc eax; mov [0x2020], eax
    char code[] = "\xA1\x20\x20\x00\x00\x04\x00\x00\x00\xFF\xC0\xA3\x20\x20\x00"
                  "\x00\x04\x00\x00\x00";

    OK(uc_open(UC_ARCH_X86, UC_MODE_64, &uc));

    // Allocate contexts
    OK(uc_context_alloc(uc, &c0));
    OK(uc_context_alloc(uc, &c1));
    OK(uc_ctl_context_mode(uc, UC_CTL_CONTEXT_MEMORY));

    OK(uc_ctl_tlb_mode(uc, UC_TLB_VIRTUAL));
    OK(uc_hook_add(uc, &hook, UC_HOOK_TLB_FILL,
                   test_snapshot_with_vtlb_callback, NULL, 1, 0));

    // Map physical memory
    OK(uc_mem_map(uc, 0x1000, 0x1000, UC_PROT_EXEC | UC_PROT_READ));
    OK(uc_mem_write(uc, 0x1000, code, sizeof(code) - 1));
    OK(uc_mem_map(uc, 0x2000, 0x1000, UC_PROT_ALL));

    // Initial context save
    OK(uc_context_save(uc, c0));

    OK(uc_emu_start(uc, 0x400000000 + 0x1000,
                    0x400000000 + 0x1000 + sizeof(code) - 1, 0, 0));
    OK(uc_mem_read(uc, 0x2020, &mem, sizeof(mem)));
    TEST_CHECK(LEINT32(mem) == 1);
    OK(uc_context_save(uc, c1));
    OK(uc_emu_start(uc, 0x400000000 + 0x1000,
                    0x400000000 + 0x1000 + sizeof(code) - 1, 0, 0));
    OK(uc_mem_read(uc, 0x2020, &mem, sizeof(mem)));
    TEST_CHECK(LEINT32(mem) == 2);
    OK(uc_context_restore(uc, c1));
    // TODO check mem
    OK(uc_mem_read(uc, 0x2020, &mem, sizeof(mem)));
    TEST_CHECK(LEINT32(mem) == 1);
    OK(uc_context_restore(uc, c0));
    OK(uc_mem_read(uc, 0x2020, &mem, sizeof(mem)));
    TEST_CHECK(LEINT32(mem) == 0);
    // TODO check mem

    OK(uc_context_free(c0));
    OK(uc_context_free(c1));
    OK(uc_close(uc));
}

static void test_context_snapshot(void)
{
    uc_engine *uc;
    uc_context *ctx;
    uint64_t baseaddr = 0xfffff1000;
    uint64_t offset = 0x10;
    uint64_t tmp = 1;
    OK(uc_open(UC_ARCH_X86, UC_MODE_64, &uc));
    OK(uc_ctl_context_mode(uc, UC_CTL_CONTEXT_MEMORY | UC_CTL_CONTEXT_CPU));
    OK(uc_mem_map(uc, baseaddr, 0x1000, UC_PROT_ALL));
    OK(uc_context_alloc(uc, &ctx));
    OK(uc_context_save(uc, ctx));

    OK(uc_mem_write(uc, baseaddr + offset, &tmp, sizeof(tmp)));
    OK(uc_mem_read(uc, baseaddr + offset, &tmp, sizeof(tmp)));
    TEST_CHECK(tmp == 1);
    OK(uc_context_restore(uc, ctx));
    OK(uc_mem_read(uc, baseaddr + offset, &tmp, sizeof(tmp)));
    TEST_CHECK(tmp == 0);

    tmp = 2;
    OK(uc_mem_write(uc, baseaddr + offset, &tmp, sizeof(tmp)));
    OK(uc_mem_read(uc, baseaddr + offset, &tmp, sizeof(tmp)));
    TEST_CHECK(tmp == 2);
    OK(uc_context_restore(uc, ctx));
    OK(uc_mem_read(uc, baseaddr + offset, &tmp, sizeof(tmp)));
    TEST_CHECK(tmp == 0);

    OK(uc_context_free(ctx));
    OK(uc_close(uc));
}

static void test_snapshot_unmap(void)
{
    uc_engine *uc;
    uc_context *ctx;
    uint64_t tmp;

    OK(uc_open(UC_ARCH_X86, UC_MODE_64, &uc));
    OK(uc_ctl_context_mode(uc, UC_CTL_CONTEXT_MEMORY | UC_CTL_CONTEXT_CPU));
    OK(uc_mem_map(uc, 0x1000, 0x2000, UC_PROT_ALL));

    tmp = 1;
    OK(uc_mem_write(uc, 0x1000, &tmp, sizeof(tmp)));
    tmp = 2;
    OK(uc_mem_write(uc, 0x2000, &tmp, sizeof(tmp)));

    OK(uc_context_alloc(uc, &ctx));
    OK(uc_context_save(uc, ctx));

    uc_assert_err(UC_ERR_ARG, uc_mem_unmap(uc, 0x1000, 0x1000));
    OK(uc_mem_unmap(uc, 0x1000, 0x2000));
    uc_assert_err(UC_ERR_READ_UNMAPPED,
                  uc_mem_read(uc, 0x1000, &tmp, sizeof(tmp)));
    uc_assert_err(UC_ERR_READ_UNMAPPED,
                  uc_mem_read(uc, 0x2000, &tmp, sizeof(tmp)));

    OK(uc_context_restore(uc, ctx));
    OK(uc_mem_read(uc, 0x1000, &tmp, sizeof(tmp)));
    TEST_CHECK(tmp == 1);
    OK(uc_mem_read(uc, 0x2000, &tmp, sizeof(tmp)));
    TEST_CHECK(tmp == 2);

    OK(uc_context_free(ctx));
    OK(uc_close(uc));
}

static void parts_increment(size_t idx, char parts[3])
{
    if (idx && idx % 3 == 0) {
        if (++parts[2] > '9') {
            parts[2] = '0';
            if (++parts[1] > 'z') {
                parts[1] = 'a';
                if (++parts[0] > 'Z')
                    parts[0] = 'A';
            }
        }
    }
}

// Create a pattern string. It works the same as
// https://github.com/rapid7/metasploit-framework/blob/master/tools/exploit/pattern_create.rb
static void pattern_create(char *buf, size_t len)
{
    char parts[] = {'A', 'a', '0'};
    size_t i;

    for (i = 0; i < len; i++) {
        buf[i] = parts[i % 3];
        parts_increment(i, parts);
    }
}

static bool pattern_verify(const char *buf, size_t len)
{
    char parts[] = {'A', 'a', '0'};
    size_t i;

    for (i = 0; i < len; i++) {
        if (buf[i] != parts[i % 3])
            return false;
        parts_increment(i, parts);
    }

    return true;
}

// Test for reading and writing memory block that are bigger than INT_MAX.
static void test_mem_read_and_write_large_memory_block(void)
{
    uc_engine *uc;
    uint64_t mem_addr = 0x1000000;
    uint64_t mem_size = 0x9f000000;
    char *pmem = NULL;

    if (sizeof(void *) < 8) {
        // Don't perform the test on a 32-bit platforms since we may not have
        // enough memory space.
        return;
    }
    // Android CI/CD services do not have enough memory capacity for this
    // test to work. Executing it will result in a permanent loop with the
    // low memory killer daemon. 
#ifdef __ANDROID__
    return;
#endif 

    OK(uc_open(UC_ARCH_ARM64, UC_MODE_ARM, &uc));
    OK(uc_mem_map(uc, mem_addr, mem_size, UC_PROT_ALL));

    pmem = malloc(mem_size);
    if (TEST_CHECK(pmem != NULL)) {
        pattern_create(pmem, mem_size);

        OK(uc_mem_write(uc, mem_addr, pmem, mem_size));
        memset(pmem, 'a', mem_size);
        OK(uc_mem_read(uc, mem_addr, pmem, mem_size));
        TEST_CHECK(pattern_verify(pmem, mem_size));
        free(pmem);
    }

    OK(uc_mem_unmap(uc, mem_addr, mem_size));
    OK(uc_close(uc));
}

static bool test_v2p_tlb_fill(uc_engine *uc, uint64_t addr, uc_mem_type type,
                               uc_tlb_entry *result, void *user_data)               
{
    if (type != UC_MEM_READ)
        return false;
    result->paddr = addr;
    result->perms = UC_PROT_READ;
    return true;
}

static void test_virtual_to_physical(void)
{
    uc_engine *uc;
    uc_hook hook;
    uint64_t res;

    OK(uc_open(UC_ARCH_X86, UC_MODE_64, &uc));
    OK(uc_ctl_tlb_mode(uc, UC_TLB_VIRTUAL));
    OK(uc_hook_add(uc, &hook, UC_HOOK_TLB_FILL, test_v2p_tlb_fill, NULL, 1, 0));

    OK(uc_vmem_translate(uc, 0x1000, UC_PROT_READ, &res));
    uc_assert_err(UC_ERR_WRITE_PROT,
                  uc_vmem_translate(uc, 0x1000, UC_PROT_WRITE, &res));
    OK(uc_close(uc));
}

static bool test_virtual_write_tlb_fill(uc_engine *uc, uint64_t addr, uc_mem_type type,
                                        uc_tlb_entry *result, void *user_data)
{
    if (addr < 0x1000)
        return false;
    result->paddr = addr - 0x1000;
    result->perms = UC_PROT_ALL;
    return true;
}

static void test_virtual_write(void)
{
    uc_engine *uc;
    uc_hook hook;
    uint64_t rax = 21;
    uint64_t res = 0;
    /*
     * mov rax, [0x2000]
     */
    char code[] = { 0x48, 0x8B, 0x04, 0x25, 0x00, 0x20, 0x00, 0x00 };

    OK(uc_open(UC_ARCH_X86, UC_MODE_64, &uc));
    OK(uc_ctl_tlb_mode(uc, UC_TLB_VIRTUAL));
    OK(uc_hook_add(uc, &hook, UC_HOOK_TLB_FILL, test_virtual_write_tlb_fill, NULL, 1, 0));
    OK(uc_mem_map(uc, 0x0, 0x2000, UC_PROT_ALL));

    OK(uc_vmem_write(uc, 0x1000, UC_PROT_EXEC, code, sizeof(code)));
    OK(uc_vmem_write(uc, 0x2000, UC_PROT_READ, &rax, sizeof(rax)));

    OK(uc_emu_start(uc, 0x1000, 0x1000 + sizeof(code) - 1, 0, 1));
    OK(uc_reg_read(uc, UC_X86_REG_RAX, &res));
    TEST_CHECK(rax == res);

    OK(uc_close(uc));
}

TEST_LIST = {{"test_map_correct", test_map_correct},
             {"test_map_wrapping", test_map_wrapping},
             {"test_mem_protect", test_mem_protect},
             {"test_splitting_mem_unmap", test_splitting_mem_unmap},
             {"test_splitting_mmio_unmap", test_splitting_mmio_unmap},
             {"test_mem_protect_map_ptr", test_mem_protect_map_ptr},
             {"test_map_at_the_end", test_map_at_the_end},
             {"test_map_wrap", test_map_wrap},
             {"test_map_big_memory", test_map_big_memory},
             {"test_mem_protect_remove_exec", test_mem_protect_remove_exec},
             {"test_mem_protect_mmio", test_mem_protect_mmio},
             {"test_snapshot", test_snapshot},
             {"test_snapshot_with_vtlb", test_snapshot_with_vtlb},
             {"test_context_snapshot", test_context_snapshot},
             {"test_snapshot_unmap", test_snapshot_unmap},
             {"test_mem_read_and_write_large_memory_block",
              test_mem_read_and_write_large_memory_block},
             {"test_virtual_to_physical", test_virtual_to_physical},
             {"test_virtual_write", test_virtual_write},
             {NULL, NULL}};
