#include "nemu.h"
#include "device/mmio.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound, eip = %08x, ebp = %08x eax = %d", addr, cpu.eip, cpu.ebp, cpu.eax); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
	int no = is_mmio(addr);
	if (no == -1) {
		return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
	} else {
		return mmio_read(addr, len, no);
	}
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
	int no = is_mmio(addr);
	if (no == -1) {
		memcpy(guest_to_host(addr), &data, len);
	} else {
		mmio_write(addr, len, data, no);
	}
}

paddr_t page_translate(vaddr_t addr) {
  if (!(cpu.cr0 & 0x80000000)) return addr;

  uint32_t dir = ((addr >> 22) & 0x3ff) << 2;
  uint32_t page = ((addr >> 12) & 0x3ff) << 2;
  uint32_t offset = addr & 0xfff;

  uint32_t dir_base = cpu.cr3 & 0xfffff000;
  uint32_t pde = paddr_read(dir_base + dir, 4);
  if (!(pde & 0x1)) {
    printf("eip = %08X, addr = %08X, pde = %08X\n", cpu.eip, addr, pde);
    printf("cr0 = %08X, cr3 = %08X\n", cpu.cr0, cpu.cr3);
  }

  assert(pde & 0x1);
  uint32_t pte = paddr_read((pde & 0xfffff000) + page, 4);
  if (!(pte & 0x1)) {
    printf("eip = %08X, addr = %08X, pde = %08X\n", cpu.eip, addr, pde);
    printf("pte = %08X, eax = %08X\n", pte, cpu.eax);
    printf("cr0 = %08X, cr3 = %08X\n", cpu.cr0, cpu.cr3);
  }

  assert(pte & 0x1);
  return (pte & 0xfffff000) | (offset);
}

uint32_t vaddr_read(vaddr_t addr, int len) {
    if ((addr >> 12) != ((addr + len - 1) >> 12)) 
    {
        //printf("vaddr_read not align. cr0 = %08X, addr = %08X, len = %d\n", cpu.cr0, addr, len);
        int len1 = 4096 - (addr & 0xfff);
        int len2 = len - len1;
        vaddr_t addr1 = page_translate(addr);
        vaddr_t addr2 = page_translate(addr + len1);
        uint32_t ret1 = paddr_read(addr1, len1);
        uint32_t ret2 = paddr_read(addr2, len2);
        uint32_t ans = ret1 | (ret2 << (len1 << 3));
        //printf("ans = %08X\n", ans);
        return ans;
    }
    else {
        paddr_t paddr = page_translate(addr);
        return paddr_read(paddr, len);
    }
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
    if ((addr >> 12) != ((addr + len - 1) >> 12))
    {
        printf("vaddr_write not align. cr0 = %08X\n", cpu.cr0);
        int len1 = 4096 - (addr & 0xfff);
        int len2 = len - len1;
        vaddr_t addr1 = page_translate(addr);
        vaddr_t addr2 = page_translate(addr + len1);
        paddr_write(addr1, len1, data & ((1 << (len1 << 3)) - 1));
        paddr_write(addr2, len2, data >> (len1 << 3));
    }
    else {
        paddr_t paddr = page_translate(addr);
        paddr_write(paddr, len, data);
    }
}
