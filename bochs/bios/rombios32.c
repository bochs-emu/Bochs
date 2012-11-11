/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  32 bit Bochs BIOS init code
//  Copyright (C) 2006 Fabrice Bellard
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
#include <stdarg.h>
#include <stddef.h>

#include "rombios.h"

typedef signed char  int8_t;
typedef short int16_t;
typedef int   int32_t;
typedef long long int64_t;
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned long long uint64_t;

#define cpuid(index, eax, ebx, ecx, edx) \
  asm volatile ("cpuid" \
                : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx) \
                : "0" (index))

#define wbinvd() asm volatile("wbinvd")

static inline void outl(int addr, int val)
{
    asm volatile ("outl %1, %w0" : : "d" (addr), "a" (val));
}

static inline void outw(int addr, int val)
{
    asm volatile ("outw %w1, %w0" : : "d" (addr), "a" (val));
}

static inline void outb(int addr, int val)
{
    asm volatile ("outb %b1, %w0" : : "d" (addr), "a" (val));
}

static inline uint32_t inl(int addr)
{
    uint32_t val;
    asm volatile ("inl %w1, %0" : "=a" (val) : "d" (addr));
    return val;
}

static inline uint16_t inw(int addr)
{
    uint16_t val;
    asm volatile ("inw %w1, %w0" : "=a" (val) : "d" (addr));
    return val;
}

static inline uint8_t inb(int addr)
{
    uint8_t val;
    asm volatile ("inb %w1, %b0" : "=a" (val) : "d" (addr));
    return val;
}

static inline void writel(void *addr, uint32_t val)
{
    *(volatile uint32_t *)addr = val;
}

static inline void writew(void *addr, uint16_t val)
{
    *(volatile uint16_t *)addr = val;
}

static inline void writeb(void *addr, uint8_t val)
{
    *(volatile uint8_t *)addr = val;
}

static inline uint32_t readl(const void *addr)
{
    return *(volatile const uint32_t *)addr;
}

static inline uint16_t readw(const void *addr)
{
    return *(volatile const uint16_t *)addr;
}

static inline uint8_t readb(const void *addr)
{
    return *(volatile const uint8_t *)addr;
}

static inline void putch(int c)
{
    outb(INFO_PORT, c);
}

static uint64_t rdmsr(unsigned index)
{
    unsigned long long ret;

    asm ("rdmsr" : "=A"(ret) : "c"(index));
    return ret;
}

static void wrmsr(unsigned index, uint64_t val)
{
    asm volatile ("wrmsr" : : "c"(index), "A"(val));
}

static inline int isdigit(int c)
{
    return c >= '0' && c <= '9';
}

void *memset(void *d1, int val, size_t len)
{
    uint8_t *d = d1;

    while (len--) {
        *d++ = val;
    }
    return d1;
}

void *memcpy(void *d1, const void *s1, size_t len)
{
    uint8_t *d = d1;
    const uint8_t *s = s1;

    while (len--) {
        *d++ = *s++;
    }
    return d1;
}

void *memmove(void *d1, const void *s1, size_t len)
{
    uint8_t *d = d1;
    const uint8_t *s = s1;

    if (d <= s) {
        while (len--) {
            *d++ = *s++;
        }
    } else {
        d += len;
        s += len;
        while (len--) {
            *--d = *--s;
        }
    }
    return d1;
}

int memcmp(const void *s1, const void *s2, size_t len)
{
    const int8_t *p1 = s1;
    const int8_t *p2 = s2;

    while (len--) {
        int r = *p1++ - *p2++;
        if(r)
            return r;
    }

    return 0;
}

size_t strlen(const char *s)
{
    const char *s1;
    for(s1 = s; *s1 != '\0'; s1++);
    return s1 - s;
}

/* from BSD ppp sources */
int vsnprintf(char *buf, int buflen, const char *fmt, va_list args)
{
    int c, i, n;
    int width, prec, fillch;
    int base, len, neg;
    unsigned long val = 0;
    const char *f;
    char *str, *buf0;
    char num[32];
    static const char hexchars[] = "0123456789abcdef";

    buf0 = buf;
    --buflen;
    while (buflen > 0) {
	for (f = fmt; *f != '%' && *f != 0; ++f)
	    ;
	if (f > fmt) {
	    len = f - fmt;
	    if (len > buflen)
		len = buflen;
	    memcpy(buf, fmt, len);
	    buf += len;
	    buflen -= len;
	    fmt = f;
	}
	if (*fmt == 0)
	    break;
	c = *++fmt;
	width = prec = 0;
	fillch = ' ';
	if (c == '0') {
	    fillch = '0';
	    c = *++fmt;
	}
	if (c == '*') {
	    width = va_arg(args, int);
	    c = *++fmt;
	} else {
	    while (isdigit(c)) {
		width = width * 10 + c - '0';
		c = *++fmt;
	    }
	}
	if (c == '.') {
	    c = *++fmt;
	    if (c == '*') {
		prec = va_arg(args, int);
		c = *++fmt;
	    } else {
		while (isdigit(c)) {
		    prec = prec * 10 + c - '0';
		    c = *++fmt;
		}
	    }
	}
        /* modifiers */
        switch(c) {
        case 'l':
            c = *++fmt;
            break;
        default:
            break;
        }
        str = 0;
	base = 0;
	neg = 0;
	++fmt;
	switch (c) {
	case 'd':
	    i = va_arg(args, int);
	    if (i < 0) {
		neg = 1;
		val = -i;
	    } else
		val = i;
	    base = 10;
	    break;
	case 'o':
	    val = va_arg(args, unsigned int);
	    base = 8;
	    break;
	case 'x':
	case 'X':
	    val = va_arg(args, unsigned int);
	    base = 16;
	    break;
	case 'p':
	    val = (unsigned long) va_arg(args, void *);
	    base = 16;
	    neg = 2;
	    break;
	case 's':
	    str = va_arg(args, char *);
	    break;
	case 'c':
	    num[0] = va_arg(args, int);
	    num[1] = 0;
	    str = num;
	    break;
	default:
	    *buf++ = '%';
	    if (c != '%')
		--fmt;		/* so %z outputs %z etc. */
	    --buflen;
	    continue;
	}
	if (base != 0) {
	    str = num + sizeof(num);
	    *--str = 0;
	    while (str > num + neg) {
		*--str = hexchars[val % base];
		val = val / base;
		if (--prec <= 0 && val == 0)
		    break;
	    }
	    switch (neg) {
	    case 1:
		*--str = '-';
		break;
	    case 2:
		*--str = 'x';
		*--str = '0';
		break;
	    }
	    len = num + sizeof(num) - 1 - str;
	} else {
	    len = strlen(str);
	    if (prec > 0 && len > prec)
		len = prec;
	}
	if (width > 0) {
	    if (width > buflen)
		width = buflen;
	    if ((n = width - len) > 0) {
		buflen -= n;
		for (; n > 0; --n)
		    *buf++ = fillch;
	    }
	}
	if (len > buflen)
	    len = buflen;
	memcpy(buf, str, len);
	buf += len;
	buflen -= len;
    }
    *buf = 0;
    return buf - buf0;
}

int snprintf(char * buf, size_t size, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsnprintf(buf, size, fmt, args);
    va_end(args);
    return i;
}

void bios_printf(int flags, const char *fmt, ...)
{
    va_list ap;
    char buf[1024];
    const char *s;

    if ((flags & BIOS_PRINTF_DEBHALT) == BIOS_PRINTF_DEBHALT)
        outb(PANIC_PORT2, 0x00);

    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    s = buf;
    while (*s)
        putch(*s++);
    va_end(ap);
}

void delay_ms(int n)
{
    int i, j;
    for(i = 0; i < n; i++) {
#ifdef BX_QEMU
        volatile int k;
        /* approximative ! */
        for(j = 0; j < 1000000; j++) {
          k++;
        }
#else
        {
          int r1, r2;
          j = 66;
          r1 = inb(0x61) & 0x10;
          do {
            r2 = inb(0x61) & 0x10;
            if (r1 != r2) {
              j--;
              r1 = r2;
            }
          } while (j > 0);
        }
#endif
    }
}

uint16_t smp_cpus;
uint32_t cpuid_signature;
uint32_t cpuid_features;
uint32_t cpuid_ext_features;
unsigned long ram_size;
uint64_t ram_end;
uint8_t bios_uuid[16];
#ifdef BX_USE_EBDA_TABLES
unsigned long ebda_cur_addr;
#endif
int acpi_enabled;
uint32_t pm_io_base, smb_io_base;
int pm_sci_int;
unsigned long bios_table_cur_addr;
unsigned long bios_table_end_addr;

void wrmsr_smp(uint32_t index, uint64_t val)
{
    static struct { uint32_t ecx, eax, edx; } *p = (void *)SMP_MSR_ADDR;

    wrmsr(index, val);
    p->ecx = index;
    p->eax = val;
    p->edx = val >> 32;
    ++p;
    p->ecx = 0;
}

#ifdef BX_QEMU
int qemu_cfg_port;

void qemu_cfg_select(int f)
{
    outw(QEMU_CFG_CTL_PORT, f);
}

int qemu_cfg_port_probe()
{
    char *sig = "QEMU";
    int i;

    qemu_cfg_select(QEMU_CFG_SIGNATURE);

    for (i = 0; i < 4; i++)
        if (inb(QEMU_CFG_DATA_PORT) != sig[i])
            return 0;

    return 1;
}

void qemu_cfg_read(uint8_t *buf, int len)
{
    while (len--)
        *(buf++) = inb(QEMU_CFG_DATA_PORT);
}
#endif

void uuid_probe(void)
{
#ifdef BX_QEMU
    if(qemu_cfg_port) {
        qemu_cfg_select(QEMU_CFG_UUID);
        qemu_cfg_read(bios_uuid, 16);
        return;
    }
#endif
    memset(bios_uuid, 0, 16);
}

void cpu_probe(void)
{
    uint32_t eax, ebx, ecx, edx;
    cpuid(1, eax, ebx, ecx, edx);
    cpuid_signature = eax;
    cpuid_features = edx;
    cpuid_ext_features = ecx;
}

static int cmos_readb(int addr)
{
    outb(0x70, addr);
    return inb(0x71);
}

void setup_mtrr(void)
{
    int i, vcnt, fix, wc;
    uint32_t mtrr_cap;
    union {
        uint8_t valb[8];
        uint64_t val;
    } u;

    *(uint32_t *)SMP_MSR_ADDR = 0;

    if (!(cpuid_features & CPUID_MTRR))
        return;

    if (!(cpuid_features & CPUID_MSR))
        return;

    mtrr_cap = rdmsr(MSR_MTRRcap);
    vcnt = mtrr_cap & 0xff;
    fix = mtrr_cap & 0x100;
    wc = mtrr_cap & 0x400;
    if (!vcnt || !fix)
        return;

    u.val = 0;
    for (i = 0; i < 8; ++i)
        if (ram_size >= 65536 * (i + 1))
            u.valb[i] = 6;
    wrmsr_smp(MSR_MTRRfix64K_00000, u.val);
    u.val = 0;
    for (i = 0; i < 8; ++i)
        if (ram_size >= 65536 * 8 + 16384 * (i + 1))
            u.valb[i] = 6;
    wrmsr_smp(MSR_MTRRfix16K_80000, u.val);
    wrmsr_smp(MSR_MTRRfix16K_A0000, 0);
    wrmsr_smp(MSR_MTRRfix4K_C0000, 0);
    wrmsr_smp(MSR_MTRRfix4K_C8000, 0);
    wrmsr_smp(MSR_MTRRfix4K_D0000, 0);
    wrmsr_smp(MSR_MTRRfix4K_D8000, 0);
    wrmsr_smp(MSR_MTRRfix4K_E0000, 0);
    wrmsr_smp(MSR_MTRRfix4K_E8000, 0);
    wrmsr_smp(MSR_MTRRfix4K_F0000, 0);
    wrmsr_smp(MSR_MTRRfix4K_F8000, 0);
    /* Mark 3-4GB as UC, anything not specified defaults to WB */
    wrmsr_smp(MTRRphysBase_MSR(0), 0xc0000000 | MTRR_MEMTYPE_UC);
    /* Make sure no reserved bit set to '1 in MTRRphysMask_MSR */
    wrmsr_smp(MTRRphysMask_MSR(0), (uint32_t)(~(0x40000000 - 1)) | 0x800);
    wrmsr_smp(MSR_MTRRdefType, 0xc00 | MTRR_MEMTYPE_WB);
}

void ram_probe(void)
{
  if (cmos_readb(0x34) | cmos_readb(0x35))
    ram_size = (cmos_readb(0x34) | (cmos_readb(0x35) << 8)) * 65536 +
        16 * 1024 * 1024;
  else
    ram_size = (cmos_readb(0x30) | (cmos_readb(0x31) << 8)) * 1024 +
        1 * 1024 * 1024;
  BX_INFO("ram_size=0x%08lx\n", ram_size);

  if (cmos_readb(0x5b) | cmos_readb(0x5c) | cmos_readb(0x5d))
    ram_end = (((uint64_t)cmos_readb(0x5b) << 16) |
               ((uint64_t)cmos_readb(0x5c) << 24) |
               ((uint64_t)cmos_readb(0x5d) << 32)) + (1ull << 32);
  else
    ram_end = ram_size;
  BX_INFO("ram_end=%ldMB\n", ram_end >> 20);
#ifdef BX_USE_EBDA_TABLES
  ebda_cur_addr = ((*(uint16_t *)(0x40e)) << 4) + 0x386;
  BX_INFO("ebda_cur_addr: 0x%08lx\n", ebda_cur_addr);
#endif
}

/****************************************************/
/* SMP probe */

extern uint8_t smp_ap_boot_code_start;
extern uint8_t smp_ap_boot_code_end;

/* find the number of CPUs by launching a SIPI to them */
void smp_probe(void)
{
    uint32_t val, sipi_vector;

    writew(&smp_cpus, 1);
    if (cpuid_features & CPUID_APIC) {

        /* enable local APIC */
        val = readl(APIC_BASE + APIC_SVR);
        val |= APIC_ENABLED;
        writel(APIC_BASE + APIC_SVR, val);

        /* copy AP boot code */
        memcpy((void *)AP_BOOT_ADDR, &smp_ap_boot_code_start,
               &smp_ap_boot_code_end - &smp_ap_boot_code_start);

        /* broadcast SIPI */
        writel(APIC_BASE + APIC_ICR_LOW, 0x000C4500);
        sipi_vector = AP_BOOT_ADDR >> 12;
        writel(APIC_BASE + APIC_ICR_LOW, 0x000C4600 | sipi_vector);

#ifndef BX_QEMU
        delay_ms(10);
#else
        while (cmos_readb(0x5f) + 1 != readw(&smp_cpus))
            ;
#endif
    }
    BX_INFO("Found %d cpu(s)\n", readw(&smp_cpus));
}

/****************************************************/
/* PCI init */

typedef struct PCIDevice {
    int bus;
    int devfn;
} PCIDevice;

static uint32_t pci_bios_io_addr;
static uint32_t pci_bios_mem_addr;
static uint32_t pci_bios_rom_start;
/* host irqs corresponding to PCI irqs A-D */
static uint8_t pci_irqs[4] = { 11, 9, 11, 9 };
static PCIDevice i440_pcidev = {-1, -1};

static void pci_config_writel(PCIDevice *d, uint32_t addr, uint32_t val)
{
    outl(0xcf8, 0x80000000 | (d->bus << 16) | (d->devfn << 8) | (addr & 0xfc));
    outl(0xcfc, val);
}

static void pci_config_writew(PCIDevice *d, uint32_t addr, uint32_t val)
{
    outl(0xcf8, 0x80000000 | (d->bus << 16) | (d->devfn << 8) | (addr & 0xfc));
    outw(0xcfc + (addr & 2), val);
}

static void pci_config_writeb(PCIDevice *d, uint32_t addr, uint32_t val)
{
    outl(0xcf8, 0x80000000 | (d->bus << 16) | (d->devfn << 8) | (addr & 0xfc));
    outb(0xcfc + (addr & 3), val);
}

static uint32_t pci_config_readl(PCIDevice *d, uint32_t addr)
{
    outl(0xcf8, 0x80000000 | (d->bus << 16) | (d->devfn << 8) | (addr & 0xfc));
    return inl(0xcfc);
}

static uint32_t pci_config_readw(PCIDevice *d, uint32_t addr)
{
    outl(0xcf8, 0x80000000 | (d->bus << 16) | (d->devfn << 8) | (addr & 0xfc));
    return inw(0xcfc + (addr & 2));
}

static uint32_t pci_config_readb(PCIDevice *d, uint32_t addr)
{
    outl(0xcf8, 0x80000000 | (d->bus << 16) | (d->devfn << 8) | (addr & 0xfc));
    return inb(0xcfc + (addr & 3));
}

static void pci_set_io_region_addr(PCIDevice *d, int region_num, uint32_t addr)
{
    uint16_t cmd;
    uint32_t ofs, old_addr;

    if ( region_num == PCI_ROM_SLOT ) {
        ofs = PCI_ROM_ADDRESS;
        addr |= PCI_ROM_ADDRESS_ENABLE;
    }else{
        ofs = PCI_BASE_ADDRESS_0 + region_num * 4;
    }

    old_addr = pci_config_readl(d, ofs);

    pci_config_writel(d, ofs, addr);
    BX_INFO("region %d: 0x%08x\n", region_num, addr & ~0x01);

    /* enable memory mappings */
    cmd = pci_config_readw(d, PCI_COMMAND);
    if ( region_num == PCI_ROM_SLOT )
        cmd |= PCI_COMMAND_MEMORY;
    else if (old_addr & PCI_ADDRESS_SPACE_IO)
        cmd |= PCI_COMMAND_IO;
    else
        cmd |= PCI_COMMAND_MEMORY;
    pci_config_writew(d, PCI_COMMAND, cmd);
}

/* return the global irq number corresponding to a given device irq
   pin. We could also use the bus number to have a more precise
   mapping. */
static int pci_slot_get_pirq(PCIDevice *pci_dev, int irq_num)
{
    int slot_addend;
    slot_addend = (pci_dev->devfn >> 3) - 1;
    return (irq_num + slot_addend) & 3;
}

static void find_bios_table_area(void)
{
    unsigned long addr;
    for(addr = 0xf0000; addr < 0x100000; addr += 16) {
        if (*(uint32_t *)addr == 0xaafb4442) {
            bios_table_cur_addr = addr + 8;
            bios_table_end_addr = bios_table_cur_addr + *(uint32_t *)(addr + 4);
            BX_INFO("bios_table_addr: 0x%08lx end=0x%08lx\n",
                    bios_table_cur_addr, bios_table_end_addr);
            return;
        }
    }
    return;
}

static void bios_shadow_init(PCIDevice *d)
{
    int v;

    if (bios_table_cur_addr == 0)
        return;

    /* remap the BIOS to shadow RAM an keep it read/write while we
       are writing tables */
    v = pci_config_readb(d, 0x59);
    v &= 0xcf;
    pci_config_writeb(d, 0x59, v);
    memcpy((void *)BIOS_TMP_STORAGE, (void *)0x000f0000, 0x10000);
    v |= 0x30;
    pci_config_writeb(d, 0x59, v);
    memcpy((void *)0x000f0000, (void *)BIOS_TMP_STORAGE, 0x10000);

    i440_pcidev = *d;
}

static void bios_lock_shadow_ram(void)
{
    PCIDevice *d = &i440_pcidev;
    int v;

    wbinvd();
    v = pci_config_readb(d, 0x59);
    v = (v & 0x0f) | (0x10);
    pci_config_writeb(d, 0x59, v);
}

static void pci_bios_init_bridges(PCIDevice *d)
{
    uint16_t vendor_id, device_id;

    vendor_id = pci_config_readw(d, PCI_VENDOR_ID);
    device_id = pci_config_readw(d, PCI_DEVICE_ID);

    if (vendor_id == PCI_VENDOR_ID_INTEL &&
       (device_id == PCI_DEVICE_ID_INTEL_82371FB_0 ||
        device_id == PCI_DEVICE_ID_INTEL_82371SB_0 ||
        device_id == PCI_DEVICE_ID_INTEL_82371AB_0)) {
        int i, irq;
        uint8_t elcr[2];

        /* PIIX/PIIX3/PIIX4 PCI to ISA bridge */

        elcr[0] = 0x00;
        elcr[1] = 0x00;
        for(i = 0; i < 4; i++) {
            irq = pci_irqs[i];
            /* set to trigger level */
            elcr[irq >> 3] |= (1 << (irq & 7));
            /* activate irq remapping in PIIX */
            pci_config_writeb(d, 0x60 + i, irq);
        }
        outb(0x4d0, elcr[0]);
        outb(0x4d1, elcr[1]);
        BX_INFO("PIIX3/PIIX4 init: elcr=%02x %02x\n",
                elcr[0], elcr[1]);
    } else if (vendor_id == PCI_VENDOR_ID_INTEL &&
              (device_id == PCI_DEVICE_ID_INTEL_82441 || device_id == PCI_DEVICE_ID_INTEL_82437)) {
        /* i440FX / i430FX PCI bridge */
        bios_shadow_init(d);
    }
}

extern uint8_t smm_relocation_start, smm_relocation_end;
extern uint8_t smm_code_start, smm_code_end;

#ifdef BX_USE_SMM
static void smm_init(PCIDevice *d)
{
    uint32_t value;

    /* check if SMM init is already done */
    value = pci_config_readl(d, 0x58);
    if ((value & (1 << 25)) == 0) {

        /* enable the SMM memory window */
        pci_config_writeb(&i440_pcidev, 0x72, 0x02 | 0x48);

        /* save original memory content */
        memcpy((void *)0xa8000, (void *)0x38000, 0x8000);

        /* copy the SMM relocation code */
        memcpy((void *)0x38000, &smm_relocation_start,
               &smm_relocation_end - &smm_relocation_start);

        /* enable SMI generation when writing to the APMC register */
        pci_config_writel(d, 0x58, value | (1 << 25));

        /* init APM status port */
        outb(0xb3, 0x01);

        /* raise an SMI interrupt */
        outb(0xb2, 0x00);

        /* wait until SMM code executed */
        while (inb(0xb3) != 0x00);

        /* restore original memory content */
        memcpy((void *)0x38000, (void *)0xa8000, 0x8000);

        /* copy the SMM code */
        memcpy((void *)0xa8000, &smm_code_start,
               &smm_code_end - &smm_code_start);
        wbinvd();

        /* close the SMM memory window and enable normal SMM */
        pci_config_writeb(&i440_pcidev, 0x72, 0x02 | 0x08);
    }
}
#endif

static void piix4_pm_enable(PCIDevice *d)
{
        /* PIIX4 Power Management device (for ACPI) */
        pci_config_writel(d, 0x40, PM_IO_BASE | 1);
        pci_config_writeb(d, 0x80, 0x01); /* enable PM io space */
        pci_config_writel(d, 0x90, SMB_IO_BASE | 1);
        pci_config_writeb(d, 0xd2, 0x09); /* enable SMBus io space */
#ifdef BX_USE_SMM
        smm_init(d);
#endif
}

static void pci_bios_init_pcirom(PCIDevice *d, uint32_t paddr)
{
    PCIDevice d1, *i440fx = &d1;
    uint32_t tmpaddr, size;
    uint8_t reg, v;
    int copied, shift, tmpsize;

    i440fx->bus = 0;
    i440fx->devfn = 0;
    if (paddr != 0) {
        size = readb((void *)(paddr + 2));
        if (size & 0x03) {
            size &= 0xfc;
            size += 0x04;
        }
        size <<= 9;
        if ((pci_bios_rom_start + size) > 0xe0000)
            return;
        tmpaddr = pci_bios_rom_start;
        copied = 0;
        do {
            tmpsize = 0x4000 - (tmpaddr & 0x3fff);
            if ((size - copied) < tmpsize) {
                tmpsize = size - copied;
            }
            reg = 0x5a + (uint8_t)((tmpaddr >> 15) & 0x07);
            if (tmpaddr & 0x4000) {
                shift = 4;
            } else {
                shift = 0;
            }
            v = pci_config_readb(i440fx, reg);
            v = (v & (~(0x03 << shift))) | (0x02 << shift);
            pci_config_writeb(i440fx, reg, v);
            memcpy((void *)tmpaddr, (void *)(paddr + copied), tmpsize);
            v = (v & (~(0x03 << shift))) | (0x01 << shift);
            pci_config_writeb(i440fx, reg, v);
            tmpaddr += tmpsize;
            copied += tmpsize;
        } while (copied < size);
        BX_INFO("PCI ROM copied to 0x%05x (size=0x%05x)\n", pci_bios_rom_start, size);
        pci_bios_rom_start += size;
        pci_config_writeb(d, PCI_ROM_ADDRESS, 0x00);
    }
}

static void pci_bios_init_device(PCIDevice *d)
{
    PCIDevice d1, *i440fx = &d1;
    uint16_t class;
    uint32_t *paddr;
    int i, pin, pic_irq, vendor_id, device_id;

    i440fx->bus = 0;
    i440fx->devfn = 0;
    class = pci_config_readw(d, PCI_CLASS_DEVICE);
    vendor_id = pci_config_readw(d, PCI_VENDOR_ID);
    device_id = pci_config_readw(d, PCI_DEVICE_ID);
    BX_INFO("PCI: bus=%d devfn=0x%02x: vendor_id=0x%04x device_id=0x%04x class=0x%04x\n",
            d->bus, d->devfn, vendor_id, device_id, class);
    switch(class) {
    case PCI_CLASS_STORAGE_IDE:
        if (vendor_id == PCI_VENDOR_ID_INTEL &&
           (device_id == PCI_DEVICE_ID_INTEL_82371FB_1 ||
            device_id == PCI_DEVICE_ID_INTEL_82371SB_1 ||
            device_id == PCI_DEVICE_ID_INTEL_82371AB)) {
            /* PIIX3/PIIX4 IDE */
            pci_config_writew(d, 0x40, 0x8000); // enable IDE0
            pci_config_writew(d, 0x42, 0x8000); // enable IDE1
            goto default_map;
        } else {
            /* IDE: we map it as in ISA mode */
            pci_set_io_region_addr(d, 0, 0x1f0);
            pci_set_io_region_addr(d, 1, 0x3f4);
            pci_set_io_region_addr(d, 2, 0x170);
            pci_set_io_region_addr(d, 3, 0x374);
        }
        break;
    case PCI_CLASS_SYSTEM_PIC:
        if (vendor_id == PCI_VENDOR_ID_IBM) {
            /* IBM */
            if (device_id == 0x0046 || device_id == 0xFFFF) {
                /* MPIC & MPIC2 */
                pci_set_io_region_addr(d, 0, 0x80800000 + 0x00040000);
            }
        }
        break;
    case 0xff00:
        if (vendor_id == PCI_VENDOR_ID_APPLE &&
            (device_id == 0x0017 || device_id == 0x0022)) {
            /* macio bridge */
            pci_set_io_region_addr(d, 0, 0x80800000);
        }
        break;
    default:
    default_map:
        /* default memory mappings */
        for(i = 0; i < PCI_NUM_REGIONS; i++) {
            int ofs;
            uint32_t val, size ;

            if (i == PCI_ROM_SLOT) {
                ofs = PCI_ROM_ADDRESS;
                pci_config_writel(d, ofs, 0xfffffffe);
            } else {
                ofs = PCI_BASE_ADDRESS_0 + i * 4;
                pci_config_writel(d, ofs, 0xffffffff);
            }
            val = pci_config_readl(d, ofs);
            if (val != 0) {
                size = (~(val & ~0xf)) + 1;
                if (val & PCI_ADDRESS_SPACE_IO)
                    paddr = &pci_bios_io_addr;
                else
                    paddr = &pci_bios_mem_addr;
                *paddr = (*paddr + size - 1) & ~(size - 1);
                pci_set_io_region_addr(d, i, *paddr);
                if ((i == PCI_ROM_SLOT) && (class == PCI_CLASS_DISPLAY_VGA)) {
                    pci_bios_init_pcirom(d, *paddr);
                }
                *paddr += size;
            }
        }
        break;
    }

    /* map the interrupt */
    pin = pci_config_readb(d, PCI_INTERRUPT_PIN);
    if (pin != 0) {
        pin = pci_slot_get_pirq(d, pin - 1);
        pic_irq = pci_irqs[pin];
        pci_config_writeb(d, PCI_INTERRUPT_LINE, pic_irq);
    }

    if (vendor_id == PCI_VENDOR_ID_INTEL && device_id == PCI_DEVICE_ID_INTEL_82371AB_3) {
        /* PIIX4 Power Management device (for ACPI) */
        pm_io_base = PM_IO_BASE;
        smb_io_base = SMB_IO_BASE;
        // acpi sci is hardwired to 9
        pci_config_writeb(d, PCI_INTERRUPT_LINE, 9);
        pm_sci_int = pci_config_readb(d, PCI_INTERRUPT_LINE);
        piix4_pm_enable(d);
        acpi_enabled = 1;
    }
}

static void pci_bios_init_optrom(PCIDevice *d)
{
    uint32_t paddr;
    uint16_t class;

    class = pci_config_readw(d, PCI_CLASS_DEVICE);
    if (class != PCI_CLASS_DISPLAY_VGA) {
        paddr = pci_config_readl(d, PCI_ROM_ADDRESS) & 0xfffffc00;
        pci_bios_init_pcirom(d, paddr);
    }
}

void pci_for_each_device(void (*init_func)(PCIDevice *d))
{
    PCIDevice d1, *d = &d1;
    int bus, devfn;
    uint16_t vendor_id, device_id;

    for(bus = 0; bus < 1; bus++) {
        for(devfn = 0; devfn < 256; devfn++) {
            d->bus = bus;
            d->devfn = devfn;
            vendor_id = pci_config_readw(d, PCI_VENDOR_ID);
            device_id = pci_config_readw(d, PCI_DEVICE_ID);
            if (vendor_id != 0xffff || device_id != 0xffff) {
                init_func(d);
            }
        }
    }
}

void pci_bios_init(void)
{
    pci_bios_io_addr = 0xc000;
    pci_bios_mem_addr = 0xc0000000;
    pci_bios_rom_start = 0xc0000;

    pci_for_each_device(pci_bios_init_bridges);

    pci_for_each_device(pci_bios_init_device);

    pci_for_each_device(pci_bios_init_optrom);
}

/****************************************************/
/* Multi Processor table init */

static void putb(uint8_t **pp, int val)
{
    uint8_t *q;
    q = *pp;
    *q++ = val;
    *pp = q;
}

static void putstr(uint8_t **pp, const char *str)
{
    uint8_t *q;
    q = *pp;
    while (*str)
        *q++ = *str++;
    *pp = q;
}

static void putle16(uint8_t **pp, int val)
{
    uint8_t *q;
    q = *pp;
    *q++ = val;
    *q++ = val >> 8;
    *pp = q;
}

static void putle32(uint8_t **pp, int val)
{
    uint8_t *q;
    q = *pp;
    *q++ = val;
    *q++ = val >> 8;
    *q++ = val >> 16;
    *q++ = val >> 24;
    *pp = q;
}

static int mpf_checksum(const uint8_t *data, int len)
{
    int sum, i;
    sum = 0;
    for(i = 0; i < len; i++)
        sum += data[i];
    return sum & 0xff;
}

static unsigned long align(unsigned long addr, unsigned long v)
{
    return (addr + v - 1) & ~(v - 1);
}

static void mptable_init(void)
{
    uint8_t *mp_config_table, *q, *float_pointer_struct;
    int ioapic_id, i, len;
    int mp_config_table_size;

#ifdef BX_USE_EBDA_TABLES
    if (ram_size - ACPI_DATA_SIZE - MPTABLE_MAX_SIZE < 0x100000) {
        BX_INFO("Not enough memory for MPC table\n");
        return;
    }
    mp_config_table = (uint8_t *)(ram_size - ACPI_DATA_SIZE - MPTABLE_MAX_SIZE);
#else
    bios_table_cur_addr = align(bios_table_cur_addr, 16);
    mp_config_table = (uint8_t *)bios_table_cur_addr;
#endif
    q = mp_config_table;
    putstr(&q, "PCMP"); /* "PCMP signature */
    putle16(&q, 0); /* table length (patched later) */
    putb(&q, 4); /* spec rev */
    putb(&q, 0); /* checksum (patched later) */
#ifdef BX_QEMU
    putstr(&q, "QEMUCPU "); /* OEM id */
#else
    putstr(&q, "BOCHSCPU");
#endif
    putstr(&q, "0.1         "); /* vendor id */
    putle32(&q, 0); /* OEM table ptr */
    putle16(&q, 0); /* OEM table size */
    putle16(&q, smp_cpus + 18); /* entry count */
    putle32(&q, 0xfee00000); /* local APIC addr */
    putle16(&q, 0); /* ext table length */
    putb(&q, 0); /* ext table checksum */
    putb(&q, 0); /* reserved */

    for(i = 0; i < smp_cpus; i++) {
        putb(&q, 0); /* entry type = processor */
        putb(&q, i); /* APIC id */
        putb(&q, 0x11); /* local APIC version number */
        if (i == 0)
            putb(&q, 3); /* cpu flags: enabled, bootstrap cpu */
        else
            putb(&q, 1); /* cpu flags: enabled */
        if (cpuid_signature) {
            putle32(&q, cpuid_signature);
            putle32(&q, cpuid_features);
        } else {
            putb(&q, 0); /* cpu signature */
            putb(&q, 6);
            putb(&q, 0);
            putb(&q, 0);
            putle16(&q, 0x201); /* feature flags */
            putle16(&q, 0);
        }
        putle16(&q, 0); /* reserved */
        putle16(&q, 0);
        putle16(&q, 0);
        putle16(&q, 0);
    }

    /* isa bus */
    putb(&q, 1); /* entry type = bus */
    putb(&q, 0); /* bus ID */
    putstr(&q, "ISA   ");

    /* ioapic */
    ioapic_id = smp_cpus;
    putb(&q, 2); /* entry type = I/O APIC */
    putb(&q, ioapic_id); /* apic ID */
    putb(&q, 0x11); /* I/O APIC version number */
    putb(&q, 1); /* enable */
    putle32(&q, 0xfec00000); /* I/O APIC addr */

    /* irqs */
    for(i = 0; i < 16; i++) {
        /* One entry per ioapic input. Input 2 is covered by 
           irq0->inti2 override (i == 0). irq 2 is unused */
        if (i == 2)
            continue;
        putb(&q, 3); /* entry type = I/O interrupt */
        putb(&q, 0); /* interrupt type = vectored interrupt */
        putb(&q, 0); /* flags: po=0, el=0 */
        putb(&q, 0);
        putb(&q, 0); /* source bus ID = ISA */
        putb(&q, i); /* source bus IRQ */
        putb(&q, ioapic_id); /* dest I/O APIC ID */
        putb(&q, i == 0 ? 2 : i); /* dest I/O APIC interrupt in */
    }
    /* patch length */
    len = q - mp_config_table;
    mp_config_table[4] = len;
    mp_config_table[5] = len >> 8;

    mp_config_table[7] = -mpf_checksum(mp_config_table, q - mp_config_table);

    mp_config_table_size = q - mp_config_table;

#ifndef BX_USE_EBDA_TABLES
    bios_table_cur_addr += mp_config_table_size;
#endif

    /* floating pointer structure */
#ifdef BX_USE_EBDA_TABLES
    ebda_cur_addr = align(ebda_cur_addr, 16);
    float_pointer_struct = (uint8_t *)ebda_cur_addr;
#else
    bios_table_cur_addr = align(bios_table_cur_addr, 16);
    float_pointer_struct = (uint8_t *)bios_table_cur_addr;
#endif
    q = float_pointer_struct;
    putstr(&q, "_MP_");
    /* pointer to MP config table */
    putle32(&q, (unsigned long)mp_config_table);

    putb(&q, 1); /* length in 16 byte units */
    putb(&q, 4); /* MP spec revision */
    putb(&q, 0); /* checksum (patched later) */
    putb(&q, 0); /* MP feature byte 1 */

    putb(&q, 0);
    putb(&q, 0);
    putb(&q, 0);
    putb(&q, 0);
    float_pointer_struct[10] =
        -mpf_checksum(float_pointer_struct, q - float_pointer_struct);
#ifdef BX_USE_EBDA_TABLES
    ebda_cur_addr += (q - float_pointer_struct);
#else
    bios_table_cur_addr += (q - float_pointer_struct);
#endif
    BX_INFO("MP table addr=0x%08lx MPC table addr=0x%08lx size=0x%x\n",
            (unsigned long)float_pointer_struct,
            (unsigned long)mp_config_table,
            mp_config_table_size);
}

/****************************************************/
/* ACPI tables init */

/* Table structure from Linux kernel (the ACPI tables are under the
   BSD license) */

/*
 * All tables must be byte-packed to match the ACPI specification, since
 * the tables are provided by the system BIOS.
 */

#define ACPI_TABLE_HEADER_DEF   /* ACPI common table header */ \
	uint8_t                            signature [4];          /* ACPI signature (4 ASCII characters) */\
	uint32_t                             length;                 /* Length of table, in bytes, including header */\
	uint8_t                              revision;               /* ACPI Specification minor version # */\
	uint8_t                              checksum;               /* To make sum of entire table == 0 */\
	uint8_t                            oem_id [6];             /* OEM identification */\
	uint8_t                            oem_table_id [8];       /* OEM table identification */\
	uint32_t                             oem_revision;           /* OEM revision number */\
	uint8_t                            asl_compiler_id [4];    /* ASL compiler vendor ID */\
	uint32_t                             asl_compiler_revision;  /* ASL compiler revision number */


struct acpi_table_header         /* ACPI common table header */
{
	ACPI_TABLE_HEADER_DEF
} __attribute__((__packed__));

struct rsdp_descriptor         /* Root System Descriptor Pointer */
{
	uint8_t                            signature [8];          /* ACPI signature, contains "RSD PTR " */
	uint8_t                              checksum;               /* To make sum of struct == 0 */
	uint8_t                            oem_id [6];             /* OEM identification */
	uint8_t                              revision;               /* Must be 0 for 1.0, 2 for 2.0 */
	uint32_t                             rsdt_physical_address;  /* 32-bit physical address of RSDT */
	uint32_t                             length;                 /* XSDT Length in bytes including hdr */
	uint64_t                             xsdt_physical_address;  /* 64-bit physical address of XSDT */
	uint8_t                              extended_checksum;      /* Checksum of entire table */
	uint8_t                            reserved [3];           /* Reserved field must be 0 */
} __attribute__((__packed__));

/*
 * ACPI 1.0 Root System Description Table (RSDT)
 */
struct rsdt_descriptor_rev1
{
	ACPI_TABLE_HEADER_DEF                           /* ACPI common table header */
#ifdef BX_QEMU
	uint32_t                             table_offset_entry [4]; /* Array of pointers to other */
#else
	uint32_t                             table_offset_entry [3]; /* Array of pointers to other */
#endif
			 /* ACPI tables */
} __attribute__((__packed__));

/*
 * ACPI 1.0 Firmware ACPI Control Structure (FACS)
 */
struct facs_descriptor_rev1
{
	uint8_t                            signature[4];           /* ACPI Signature */
	uint32_t                             length;                 /* Length of structure, in bytes */
	uint32_t                             hardware_signature;     /* Hardware configuration signature */
	uint32_t                             firmware_waking_vector; /* ACPI OS waking vector */
	uint32_t                             global_lock;            /* Global Lock */
	uint32_t                             S4bios_f        : 1;    /* Indicates if S4BIOS support is present */
	uint32_t                             reserved1       : 31;   /* Must be 0 */
	uint8_t                              resverved3 [40];        /* Reserved - must be zero */
} __attribute__((__packed__));


/*
 * ACPI 1.0 Fixed ACPI Description Table (FADT)
 */
struct fadt_descriptor_rev1
{
	ACPI_TABLE_HEADER_DEF                           /* ACPI common table header */
	uint32_t                             firmware_ctrl;          /* Physical address of FACS */
	uint32_t                             dsdt;                   /* Physical address of DSDT */
	uint8_t                              model;                  /* System Interrupt Model */
	uint8_t                              reserved1;              /* Reserved */
	uint16_t                             sci_int;                /* System vector of SCI interrupt */
	uint32_t                             smi_cmd;                /* Port address of SMI command port */
	uint8_t                              acpi_enable;            /* Value to write to smi_cmd to enable ACPI */
	uint8_t                              acpi_disable;           /* Value to write to smi_cmd to disable ACPI */
	uint8_t                              S4bios_req;             /* Value to write to SMI CMD to enter S4BIOS state */
	uint8_t                              reserved2;              /* Reserved - must be zero */
	uint32_t                             pm1a_evt_blk;           /* Port address of Power Mgt 1a acpi_event Reg Blk */
	uint32_t                             pm1b_evt_blk;           /* Port address of Power Mgt 1b acpi_event Reg Blk */
	uint32_t                             pm1a_cnt_blk;           /* Port address of Power Mgt 1a Control Reg Blk */
	uint32_t                             pm1b_cnt_blk;           /* Port address of Power Mgt 1b Control Reg Blk */
	uint32_t                             pm2_cnt_blk;            /* Port address of Power Mgt 2 Control Reg Blk */
	uint32_t                             pm_tmr_blk;             /* Port address of Power Mgt Timer Ctrl Reg Blk */
	uint32_t                             gpe0_blk;               /* Port addr of General Purpose acpi_event 0 Reg Blk */
	uint32_t                             gpe1_blk;               /* Port addr of General Purpose acpi_event 1 Reg Blk */
	uint8_t                              pm1_evt_len;            /* Byte length of ports at pm1_x_evt_blk */
	uint8_t                              pm1_cnt_len;            /* Byte length of ports at pm1_x_cnt_blk */
	uint8_t                              pm2_cnt_len;            /* Byte Length of ports at pm2_cnt_blk */
	uint8_t                              pm_tmr_len;              /* Byte Length of ports at pm_tm_blk */
	uint8_t                              gpe0_blk_len;           /* Byte Length of ports at gpe0_blk */
	uint8_t                              gpe1_blk_len;           /* Byte Length of ports at gpe1_blk */
	uint8_t                              gpe1_base;              /* Offset in gpe model where gpe1 events start */
	uint8_t                              reserved3;              /* Reserved */
	uint16_t                             plvl2_lat;              /* Worst case HW latency to enter/exit C2 state */
	uint16_t                             plvl3_lat;              /* Worst case HW latency to enter/exit C3 state */
	uint16_t                             flush_size;             /* Size of area read to flush caches */
	uint16_t                             flush_stride;           /* Stride used in flushing caches */
	uint8_t                              duty_offset;            /* Bit location of duty cycle field in p_cnt reg */
	uint8_t                              duty_width;             /* Bit width of duty cycle field in p_cnt reg */
	uint8_t                              day_alrm;               /* Index to day-of-month alarm in RTC CMOS RAM */
	uint8_t                              mon_alrm;               /* Index to month-of-year alarm in RTC CMOS RAM */
	uint8_t                              century;                /* Index to century in RTC CMOS RAM */
	uint8_t                              reserved4;              /* Reserved */
	uint8_t                              reserved4a;             /* Reserved */
	uint8_t                              reserved4b;             /* Reserved */
#if 0
	uint32_t                             wb_invd         : 1;    /* The wbinvd instruction works properly */
	uint32_t                             wb_invd_flush   : 1;    /* The wbinvd flushes but does not invalidate */
	uint32_t                             proc_c1         : 1;    /* All processors support C1 state */
	uint32_t                             plvl2_up        : 1;    /* C2 state works on MP system */
	uint32_t                             pwr_button      : 1;    /* Power button is handled as a generic feature */
	uint32_t                             sleep_button    : 1;    /* Sleep button is handled as a generic feature, or not present */
	uint32_t                             fixed_rTC       : 1;    /* RTC wakeup stat not in fixed register space */
	uint32_t                             rtcs4           : 1;    /* RTC wakeup stat not possible from S4 */
	uint32_t                             tmr_val_ext     : 1;    /* The tmr_val width is 32 bits (0 = 24 bits) */
	uint32_t                             reserved5       : 23;   /* Reserved - must be zero */
#else
        uint32_t flags;
#endif
} __attribute__((__packed__));

/*
 * MADT values and structures
 */

/* Values for MADT PCATCompat */

#define DUAL_PIC                0
#define MULTIPLE_APIC           1


/* Master MADT */

struct multiple_apic_table
{
	ACPI_TABLE_HEADER_DEF                           /* ACPI common table header */
	uint32_t                             local_apic_address;     /* Physical address of local APIC */
#if 0
	uint32_t                             PCATcompat      : 1;    /* A one indicates system also has dual 8259s */
	uint32_t                             reserved1       : 31;
#else
        uint32_t                             flags;
#endif
} __attribute__((__packed__));


/* Values for Type in APIC_HEADER_DEF */

#define APIC_PROCESSOR          0
#define APIC_IO                 1
#define APIC_XRUPT_OVERRIDE     2
#define APIC_NMI                3
#define APIC_LOCAL_NMI          4
#define APIC_ADDRESS_OVERRIDE   5
#define APIC_IO_SAPIC           6
#define APIC_LOCAL_SAPIC        7
#define APIC_XRUPT_SOURCE       8
#define APIC_RESERVED           9           /* 9 and greater are reserved */

/*
 * MADT sub-structures (Follow MULTIPLE_APIC_DESCRIPTION_TABLE)
 */
#define APIC_HEADER_DEF                     /* Common APIC sub-structure header */\
	uint8_t                              type; \
	uint8_t                              length;

/* Sub-structures for MADT */

struct madt_processor_apic
{
	APIC_HEADER_DEF
	uint8_t                              processor_id;           /* ACPI processor id */
	uint8_t                              local_apic_id;          /* Processor's local APIC id */
#if 0
	uint32_t                             processor_enabled: 1;   /* Processor is usable if set */
	uint32_t                             reserved2       : 31;   /* Reserved, must be zero */
#else
        uint32_t flags;
#endif
} __attribute__((__packed__));

#ifdef BX_QEMU
/*
 *  * ACPI 2.0 Generic Address Space definition.
 *   */
struct acpi_20_generic_address {
    uint8_t  address_space_id;
    uint8_t  register_bit_width;
    uint8_t  register_bit_offset;
    uint8_t  reserved;
    uint64_t address;
} __attribute__((__packed__));

/*
 *  * HPET Description Table
 *   */
struct acpi_20_hpet {
    ACPI_TABLE_HEADER_DEF                           /* ACPI common table header */
    uint32_t           timer_block_id;
    struct acpi_20_generic_address addr;
    uint8_t            hpet_number;
    uint16_t           min_tick;
    uint8_t            page_protect;
} __attribute__((__packed__));
#define ACPI_HPET_ADDRESS 0xFED00000UL
#endif

struct madt_io_apic
{
	APIC_HEADER_DEF
	uint8_t                              io_apic_id;             /* I/O APIC ID */
	uint8_t                              reserved;               /* Reserved - must be zero */
	uint32_t                             address;                /* APIC physical address */
	uint32_t                             interrupt;              /* Global system interrupt where INTI
			  * lines start */
} __attribute__((__packed__));

struct madt_int_override
{
	APIC_HEADER_DEF
	uint8_t                bus;     /* Identifies ISA Bus */
	uint8_t                source;  /* Bus-relative interrupt source */
	uint32_t               gsi;     /* GSI that source will signal */
	uint16_t               flags;   /* MPS INTI flags */
} __attribute__((__packed__));

#include "acpi-dsdt.hex"

static inline uint16_t cpu_to_le16(uint16_t x)
{
    return x;
}

static inline uint32_t cpu_to_le32(uint32_t x)
{
    return x;
}

static int acpi_checksum(const uint8_t *data, int len)
{
    int sum, i;
    sum = 0;
    for(i = 0; i < len; i++)
        sum += data[i];
    return (-sum) & 0xff;
}

static void acpi_build_table_header(struct acpi_table_header *h,
                                    char *sig, int len, uint8_t rev)
{
    memcpy(h->signature, sig, 4);
    h->length = cpu_to_le32(len);
    h->revision = rev;
#ifdef BX_QEMU
    memcpy(h->oem_id, "QEMU  ", 6);
    memcpy(h->oem_table_id, "QEMU", 4);
#else
    memcpy(h->oem_id, "BOCHS ", 6);
    memcpy(h->oem_table_id, "BXPC", 4);
#endif
    memcpy(h->oem_table_id + 4, sig, 4);
    h->oem_revision = cpu_to_le32(1);
#ifdef BX_QEMU
    memcpy(h->asl_compiler_id, "QEMU", 4);
#else
    memcpy(h->asl_compiler_id, "BXPC", 4);
#endif
    h->asl_compiler_revision = cpu_to_le32(1);
    h->checksum = acpi_checksum((void *)h, len);
}

int acpi_build_processor_ssdt(uint8_t *ssdt)
{
    uint8_t *ssdt_ptr = ssdt;
    int i, length;
    int acpi_cpus = smp_cpus > 0xff ? 0xff : smp_cpus;

    ssdt_ptr[9] = 0; // checksum;
    ssdt_ptr += sizeof(struct acpi_table_header);

    // caluculate the length of processor block and scope block excluding PkgLength
    length = 0x0d * acpi_cpus + 4;

    // build processor scope header
    *(ssdt_ptr++) = 0x10; // ScopeOp
    if (length <= 0x3e) {
        /* Handle 1-4 CPUs with one byte encoding */
        *(ssdt_ptr++) = length + 1;
    } else {
        /* Handle 5-314 CPUs with two byte encoding */
        *(ssdt_ptr++) = 0x40 | ((length + 2) & 0xf);
        *(ssdt_ptr++) = (length + 2) >> 4;
    }
    *(ssdt_ptr++) = '_'; // Name
    *(ssdt_ptr++) = 'P';
    *(ssdt_ptr++) = 'R';
    *(ssdt_ptr++) = '_';

    // build object for each processor
    for(i=0;i<acpi_cpus;i++) {
        *(ssdt_ptr++) = 0x5B; // ProcessorOp
        *(ssdt_ptr++) = 0x83;
        *(ssdt_ptr++) = 0x0B; // Length
        *(ssdt_ptr++) = 'C';  // Name (CPUxx)
        *(ssdt_ptr++) = 'P';
        if ((i & 0xf0) != 0)
            *(ssdt_ptr++) = (i >> 4) < 0xa ? (i >> 4) + '0' : (i >> 4) + 'A' - 0xa;
        else
            *(ssdt_ptr++) = 'U';
        *(ssdt_ptr++) = (i & 0xf) < 0xa ? (i & 0xf) + '0' : (i & 0xf) + 'A' - 0xa;
        *(ssdt_ptr++) = i;
        *(ssdt_ptr++) = 0x10; // Processor block address
        *(ssdt_ptr++) = 0xb0;
        *(ssdt_ptr++) = 0;
        *(ssdt_ptr++) = 0;
        *(ssdt_ptr++) = 6;    // Processor block length
    }

    acpi_build_table_header((struct acpi_table_header *)ssdt,
                            "SSDT", ssdt_ptr - ssdt, 1);

    return ssdt_ptr - ssdt;
}

/* base_addr must be a multiple of 4KB */
void acpi_bios_init(void)
{
    struct rsdp_descriptor *rsdp;
    struct rsdt_descriptor_rev1 *rsdt;
    struct fadt_descriptor_rev1 *fadt;
    struct facs_descriptor_rev1 *facs;
    struct multiple_apic_table *madt;
    uint8_t *dsdt, *ssdt;
#ifdef BX_QEMU
    struct acpi_20_hpet *hpet;
    uint32_t hpet_addr;
#endif
    uint32_t base_addr, rsdt_addr, fadt_addr, addr, facs_addr, dsdt_addr, ssdt_addr;
    uint32_t acpi_tables_size, madt_addr, madt_size;
    int i;

    if (ram_size - ACPI_DATA_SIZE < 0x100000) {
        BX_INFO("Not enough memory for ACPI tables\n");
        return;
    }

    /* reserve memory space for tables */
#ifdef BX_USE_EBDA_TABLES
    ebda_cur_addr = align(ebda_cur_addr, 16);
    rsdp = (void *)(ebda_cur_addr);
    ebda_cur_addr += sizeof(*rsdp);
#else
    bios_table_cur_addr = align(bios_table_cur_addr, 16);
    rsdp = (void *)(bios_table_cur_addr);
    bios_table_cur_addr += sizeof(*rsdp);
#endif

    addr = base_addr = ram_size - ACPI_DATA_SIZE;
    rsdt_addr = addr;
    rsdt = (void *)(addr);
    addr += sizeof(*rsdt);

    fadt_addr = addr;
    fadt = (void *)(addr);
    addr += sizeof(*fadt);

    /* XXX: FACS should be in RAM */
    addr = (addr + 63) & ~63; /* 64 byte alignment for FACS */
    facs_addr = addr;
    facs = (void *)(addr);
    addr += sizeof(*facs);

    dsdt_addr = addr;
    dsdt = (void *)(addr);
    addr += sizeof(AmlCode);

    ssdt_addr = addr;
    ssdt = (void *)(addr);
    addr += acpi_build_processor_ssdt(ssdt);

    addr = (addr + 7) & ~7;
    madt_addr = addr;
    madt_size = sizeof(*madt) +
        sizeof(struct madt_processor_apic) * smp_cpus +
        sizeof(struct madt_io_apic) + sizeof(struct madt_int_override);
    madt = (void *)(addr);
    addr += madt_size;

#ifdef BX_QEMU
    addr = (addr + 7) & ~7;
    hpet_addr = addr;
    hpet = (void *)(addr);
    addr += sizeof(*hpet);
#endif

    acpi_tables_size = addr - base_addr;

    BX_INFO("ACPI tables: RSDP addr=0x%08lx ACPI DATA addr=0x%08lx size=0x%x\n",
            (unsigned long)rsdp,
            (unsigned long)rsdt, acpi_tables_size);

    /* RSDP */
    memset(rsdp, 0, sizeof(*rsdp));
    memcpy(rsdp->signature, "RSD PTR ", 8);
#ifdef BX_QEMU
    memcpy(rsdp->oem_id, "QEMU  ", 6);
#else
    memcpy(rsdp->oem_id, "BOCHS ", 6);
#endif
    rsdp->rsdt_physical_address = cpu_to_le32(rsdt_addr);
    rsdp->checksum = acpi_checksum((void *)rsdp, 20);

    /* RSDT */
    memset(rsdt, 0, sizeof(*rsdt));
    rsdt->table_offset_entry[0] = cpu_to_le32(fadt_addr);
    rsdt->table_offset_entry[1] = cpu_to_le32(madt_addr);
    rsdt->table_offset_entry[2] = cpu_to_le32(ssdt_addr);
#ifdef BX_QEMU
    rsdt->table_offset_entry[3] = cpu_to_le32(hpet_addr);
#endif
    acpi_build_table_header((struct acpi_table_header *)rsdt,
                            "RSDT", sizeof(*rsdt), 1);

    /* FADT */
    memset(fadt, 0, sizeof(*fadt));
    fadt->firmware_ctrl = cpu_to_le32(facs_addr);
    fadt->dsdt = cpu_to_le32(dsdt_addr);
    fadt->model = 1;
    fadt->reserved1 = 0;
    fadt->sci_int = cpu_to_le16(pm_sci_int);
    fadt->smi_cmd = cpu_to_le32(SMI_CMD_IO_ADDR);
    fadt->acpi_enable = 0xf1;
    fadt->acpi_disable = 0xf0;
    fadt->pm1a_evt_blk = cpu_to_le32(pm_io_base);
    fadt->pm1a_cnt_blk = cpu_to_le32(pm_io_base + 0x04);
    fadt->pm_tmr_blk = cpu_to_le32(pm_io_base + 0x08);
    fadt->pm1_evt_len = 4;
    fadt->pm1_cnt_len = 2;
    fadt->pm_tmr_len = 4;
    fadt->plvl2_lat = cpu_to_le16(0xfff); // C2 state not supported
    fadt->plvl3_lat = cpu_to_le16(0xfff); // C3 state not supported
    /* WBINVD + PROC_C1 + PWR_BUTTON + SLP_BUTTON + FIX_RTC */
    fadt->flags = cpu_to_le32((1 << 0) | (1 << 2) | (1 << 4) | (1 << 5) | (1 << 6));
    acpi_build_table_header((struct acpi_table_header *)fadt, "FACP",
                            sizeof(*fadt), 1);

    /* FACS */
    memset(facs, 0, sizeof(*facs));
    memcpy(facs->signature, "FACS", 4);
    facs->length = cpu_to_le32(sizeof(*facs));
    BX_INFO("Firmware waking vector %p\n", &facs->firmware_waking_vector);

    /* DSDT */
    memcpy(dsdt, AmlCode, sizeof(AmlCode));

    /* MADT */
    {
        struct madt_processor_apic *apic;
        struct madt_io_apic *io_apic;
        struct madt_int_override *int_override;

        memset(madt, 0, madt_size);
        madt->local_apic_address = cpu_to_le32(0xfee00000);
        madt->flags = cpu_to_le32(1);
        apic = (void *)(madt + 1);
        for(i=0;i<smp_cpus;i++) {
            apic->type = APIC_PROCESSOR;
            apic->length = sizeof(*apic);
            apic->processor_id = i;
            apic->local_apic_id = i;
            apic->flags = cpu_to_le32(1);
            apic++;
        }
        io_apic = (void *)apic;
        io_apic->type = APIC_IO;
        io_apic->length = sizeof(*io_apic);
        io_apic->io_apic_id = smp_cpus;
        io_apic->address = cpu_to_le32(0xfec00000);
        io_apic->interrupt = cpu_to_le32(0);
        io_apic++;

        int_override = (void *)io_apic;
        int_override->type = APIC_XRUPT_OVERRIDE;
        int_override->length = sizeof(*int_override);
        int_override->bus = cpu_to_le32(0);
        int_override->source = cpu_to_le32(0);
        int_override->gsi = cpu_to_le32(2);
        int_override->flags = cpu_to_le32(0);

        acpi_build_table_header((struct acpi_table_header *)madt,
                                "APIC", madt_size, 1);
    }

#ifdef BX_QEMU
    /* HPET */
    memset(hpet, 0, sizeof(*hpet));
    /* Note timer_block_id value must be kept in sync with value advertised by
     * emulated hpet
     */
    hpet->timer_block_id = cpu_to_le32(0x8086a201);
    hpet->addr.address = cpu_to_le32(ACPI_HPET_ADDRESS);
    acpi_build_table_header((struct  acpi_table_header *)hpet,
                             "HPET", sizeof(*hpet), 1);
#endif

}

/* SMBIOS entry point -- must be written to a 16-bit aligned address
   between 0xf0000 and 0xfffff.
 */
struct smbios_entry_point {
	char anchor_string[4];
	uint8_t checksum;
	uint8_t length;
	uint8_t smbios_major_version;
	uint8_t smbios_minor_version;
	uint16_t max_structure_size;
	uint8_t entry_point_revision;
	uint8_t formatted_area[5];
	char intermediate_anchor_string[5];
	uint8_t intermediate_checksum;
	uint16_t structure_table_length;
	uint32_t structure_table_address;
	uint16_t number_of_structures;
	uint8_t smbios_bcd_revision;
} __attribute__((__packed__));

/* This goes at the beginning of every SMBIOS structure. */
struct smbios_structure_header {
	uint8_t type;
	uint8_t length;
	uint16_t handle;
} __attribute__((__packed__));

/* SMBIOS type 0 - BIOS Information */
struct smbios_type_0 {
	struct smbios_structure_header header;
	uint8_t vendor_str;
	uint8_t bios_version_str;
	uint16_t bios_starting_address_segment;
	uint8_t bios_release_date_str;
	uint8_t bios_rom_size;
	uint8_t bios_characteristics[8];
	uint8_t bios_characteristics_extension_bytes[2];
	uint8_t system_bios_major_release;
	uint8_t system_bios_minor_release;
	uint8_t embedded_controller_major_release;
	uint8_t embedded_controller_minor_release;
} __attribute__((__packed__));

/* SMBIOS type 1 - System Information */
struct smbios_type_1 {
	struct smbios_structure_header header;
	uint8_t manufacturer_str;
	uint8_t product_name_str;
	uint8_t version_str;
	uint8_t serial_number_str;
	uint8_t uuid[16];
	uint8_t wake_up_type;
	uint8_t sku_number_str;
	uint8_t family_str;
} __attribute__((__packed__));

/* SMBIOS type 3 - System Enclosure (v2.3) */
struct smbios_type_3 {
	struct smbios_structure_header header;
	uint8_t manufacturer_str;
	uint8_t type;
	uint8_t version_str;
	uint8_t serial_number_str;
	uint8_t asset_tag_number_str;
	uint8_t boot_up_state;
	uint8_t power_supply_state;
	uint8_t thermal_state;
	uint8_t security_status;
    uint32_t oem_defined;
    uint8_t height;
    uint8_t number_of_power_cords;
    uint8_t contained_element_count;
    // contained elements follow
} __attribute__((__packed__));

/* SMBIOS type 4 - Processor Information (v2.0) */
struct smbios_type_4 {
	struct smbios_structure_header header;
	uint8_t socket_designation_str;
	uint8_t processor_type;
	uint8_t processor_family;
	uint8_t processor_manufacturer_str;
	uint32_t processor_id[2];
	uint8_t processor_version_str;
	uint8_t voltage;
	uint16_t external_clock;
	uint16_t max_speed;
	uint16_t current_speed;
	uint8_t status;
	uint8_t processor_upgrade;
        uint16_t l1_cache_handle;
        uint16_t l2_cache_handle;
        uint16_t l3_cache_handle;
} __attribute__((__packed__));

/* SMBIOS type 16 - Physical Memory Array
 *   Associated with one type 17 (Memory Device).
 */
struct smbios_type_16 {
	struct smbios_structure_header header;
	uint8_t location;
	uint8_t use;
	uint8_t error_correction;
	uint32_t maximum_capacity;
	uint16_t memory_error_information_handle;
	uint16_t number_of_memory_devices;
} __attribute__((__packed__));

/* SMBIOS type 17 - Memory Device
 *   Associated with one type 19
 */
struct smbios_type_17 {
	struct smbios_structure_header header;
	uint16_t physical_memory_array_handle;
	uint16_t memory_error_information_handle;
	uint16_t total_width;
	uint16_t data_width;
	uint16_t size;
	uint8_t form_factor;
	uint8_t device_set;
	uint8_t device_locator_str;
	uint8_t bank_locator_str;
	uint8_t memory_type;
	uint16_t type_detail;
} __attribute__((__packed__));

/* SMBIOS type 19 - Memory Array Mapped Address */
struct smbios_type_19 {
	struct smbios_structure_header header;
	uint32_t starting_address;
	uint32_t ending_address;
	uint16_t memory_array_handle;
	uint8_t partition_width;
} __attribute__((__packed__));

/* SMBIOS type 20 - Memory Device Mapped Address */
struct smbios_type_20 {
	struct smbios_structure_header header;
	uint32_t starting_address;
	uint32_t ending_address;
	uint16_t memory_device_handle;
	uint16_t memory_array_mapped_address_handle;
	uint8_t partition_row_position;
	uint8_t interleave_position;
	uint8_t interleaved_data_depth;
} __attribute__((__packed__));

/* SMBIOS type 32 - System Boot Information */
struct smbios_type_32 {
	struct smbios_structure_header header;
	uint8_t reserved[6];
	uint8_t boot_status;
} __attribute__((__packed__));

/* SMBIOS type 127 -- End-of-table */
struct smbios_type_127 {
	struct smbios_structure_header header;
} __attribute__((__packed__));

static void
smbios_entry_point_init(void *start,
                        uint16_t max_structure_size,
                        uint16_t structure_table_length,
                        uint32_t structure_table_address,
                        uint16_t number_of_structures)
{
    uint8_t sum;
    int i;
    struct smbios_entry_point *ep = (struct smbios_entry_point *)start;

    memcpy(ep->anchor_string, "_SM_", 4);
    ep->length = 0x1f;
    ep->smbios_major_version = 2;
    ep->smbios_minor_version = 4;
    ep->max_structure_size = max_structure_size;
    ep->entry_point_revision = 0;
    memset(ep->formatted_area, 0, 5);
    memcpy(ep->intermediate_anchor_string, "_DMI_", 5);

    ep->structure_table_length = structure_table_length;
    ep->structure_table_address = structure_table_address;
    ep->number_of_structures = number_of_structures;
    ep->smbios_bcd_revision = 0x24;

    ep->checksum = 0;
    ep->intermediate_checksum = 0;

    sum = 0;
    for (i = 0; i < 0x10; i++)
        sum += ((int8_t *)start)[i];
    ep->checksum = -sum;

    sum = 0;
    for (i = 0x10; i < ep->length; i++)
        sum += ((int8_t *)start)[i];
    ep->intermediate_checksum = -sum;
    }

/* Type 0 -- BIOS Information */
#define RELEASE_DATE_STR "01/01/2007"
static void *
smbios_type_0_init(void *start)
{
    struct smbios_type_0 *p = (struct smbios_type_0 *)start;

    p->header.type = 0;
    p->header.length = sizeof(struct smbios_type_0);
    p->header.handle = 0;

    p->vendor_str = 1;
    p->bios_version_str = 2;
    p->bios_starting_address_segment = 0xe000;
    p->bios_release_date_str = 3;
    p->bios_rom_size = 1; /* 128 kB */

    memset(p->bios_characteristics, 0, 8);
    p->bios_characteristics[0] |= 1 << 4; /* Bit 4 - ISA is supported */
#if BX_PCIBIOS
    p->bios_characteristics[0] |= 1 << 7; /* Bit 7 - PCI is supported */
#endif
#if BX_APM
    p->bios_characteristics[1] |= 1 << 2; /* Bit 10 - APM is supported */
#endif
    p->bios_characteristics[1] |= 1 << 3; /* Bit 11 - BIOS is Upgradeable (Flash) */
    p->bios_characteristics[1] |= 1 << 4; /* Bit 12 - BIOS shadowing is allowed */
#if BX_ELTORITO_BOOT && BX_USE_ATADRV
    p->bios_characteristics[1] |= 1 << 7; /* Bit 15 - Boot from CD is supported */
    p->bios_characteristics[2] |= 1 << 0; /* Bit 16 - Selectable Boot is supported */
#endif
#if BX_USE_ATADRV
    p->bios_characteristics[2] |= 1 << 3; /* Bit 19 - EDD (Enhanced Disk Drive) Specification is supported */
#endif
#if BX_SUPPORT_FLOPPY
    p->bios_characteristics[2] |= 1 << 6; /* Bit 22 - Int 13h - 5.25" / 360 KB Floppy Services are supported */
    p->bios_characteristics[2] |= 1 << 7; /* Bit 23 - Int 13h - 5.25" / 1.2 MB Floppy Services are supported */
    p->bios_characteristics[3] |= 1 << 0; /* Bit 24 - Int 13h - 3.5" / 720 KB Floppy Services are supported */
    p->bios_characteristics[3] |= 1 << 1; /* Bit 25 - Int 13h - 3.5" / 2.88 MB Floppy Services are supported */
#endif
    p->bios_characteristics[3] |= 1 << 3; /* Bit 27 - Int 9h, 8042 Keyboard services are supported */
    p->bios_characteristics[3] |= 1 << 4; /* Bit 28 - Int 14h, Serial Services are supported */
    p->bios_characteristics[3] |= 1 << 5; /* Bit 29 - Int 17h, Printer Services are supported */
    p->bios_characteristics_extension_bytes[0] = 1; /* Bit 0 - ACPI supported */
    p->bios_characteristics_extension_bytes[1] = 0;

    p->system_bios_major_release = 1;
    p->system_bios_minor_release = 0;
    p->embedded_controller_major_release = 0xff;
    p->embedded_controller_minor_release = 0xff;

    start += sizeof(struct smbios_type_0);
    memcpy((char *)start, BX_APPVENDOR, sizeof(BX_APPVENDOR));
    start += sizeof(BX_APPVENDOR);
    memcpy((char *)start, BX_APPNAME, sizeof(BX_APPNAME));
    start += sizeof(BX_APPNAME);
    memcpy((char *)start, RELEASE_DATE_STR, sizeof(RELEASE_DATE_STR));
    start += sizeof(RELEASE_DATE_STR);
    *((uint8_t *)start) = 0;

    return start+1;
}

/* Type 1 -- System Information */
static void *
smbios_type_1_init(void *start)
{
    struct smbios_type_1 *p = (struct smbios_type_1 *)start;
    p->header.type = 1;
    p->header.length = sizeof(struct smbios_type_1);
    p->header.handle = 0x100;

    p->manufacturer_str = 0;
    p->product_name_str = 0;
    p->version_str = 0;
    p->serial_number_str = 0;

    memcpy(p->uuid, bios_uuid, 16);

    p->wake_up_type = 0x06; /* power switch */
    p->sku_number_str = 0;
    p->family_str = 0;

    start += sizeof(struct smbios_type_1);
    *((uint16_t *)start) = 0;

    return start+2;
}

/* Type 3 -- System Enclosure */
static void *
smbios_type_3_init(void *start)
{
    struct smbios_type_3 *p = (struct smbios_type_3 *)start;

    p->header.type = 3;
    p->header.length = sizeof(struct smbios_type_3);
    p->header.handle = 0x300;

    p->manufacturer_str = 0;
    p->type = 0x01; /* other */
    p->version_str = 0;
    p->serial_number_str = 0;
    p->asset_tag_number_str = 0;
    p->boot_up_state = 0x03; /* safe */
    p->power_supply_state = 0x03; /* safe */
    p->thermal_state = 0x03; /* safe */
    p->security_status = 0x02; /* unknown */
    p->oem_defined = 0;
    p->height = 0;
    p->number_of_power_cords = 0;
    p->contained_element_count = 0;

    start += sizeof(struct smbios_type_3);
    *((uint16_t *)start) = 0;

    return start+2;
}

/* Type 4 -- Processor Information */
static void *
smbios_type_4_init(void *start, unsigned int cpu_number)
{
    struct smbios_type_4 *p = (struct smbios_type_4 *)start;

    p->header.type = 4;
    p->header.length = sizeof(struct smbios_type_4);
    p->header.handle = 0x400 + cpu_number;

    p->socket_designation_str = 1;
    p->processor_type = 0x03; /* CPU */
    p->processor_family = 0x01; /* other */
    p->processor_manufacturer_str = 0;

    p->processor_id[0] = cpuid_signature;
    p->processor_id[1] = cpuid_features;

    p->processor_version_str = 0;
    p->voltage = 0;
    p->external_clock = 0;

    p->max_speed = 0; /* unknown */
    p->current_speed = 0; /* unknown */

    p->status = 0x41; /* socket populated, CPU enabled */
    p->processor_upgrade = 0x01; /* other */

    p->l1_cache_handle = 0xffff; /* cache information structure not provided */
    p->l2_cache_handle = 0xffff;
    p->l3_cache_handle = 0xffff;

    start += sizeof(struct smbios_type_4);

    memcpy((char *)start, "CPU  " "\0" "" "\0" "", 7);
	((char *)start)[4] = cpu_number + '0';

    return start+7;
}

/* Type 16 -- Physical Memory Array */
static void *
smbios_type_16_init(void *start, uint32_t memsize, int nr_mem_devs)
{
    struct smbios_type_16 *p = (struct smbios_type_16*)start;

    p->header.type = 16;
    p->header.length = sizeof(struct smbios_type_16);
    p->header.handle = 0x1000;

    p->location = 0x03; /* system board or motherboard */
    p->use = 0x03; /* system memory */
    p->error_correction = 0x01; /* other */
    p->maximum_capacity = memsize * 1024;
    p->memory_error_information_handle = 0xfffe; /* none provided */
    p->number_of_memory_devices = nr_mem_devs;

    start += sizeof(struct smbios_type_16);
    *((uint16_t *)start) = 0;

    return start + 2;
}

/* Type 17 -- Memory Device */
static void *
smbios_type_17_init(void *start, uint32_t memory_size_mb, int instance)
{
    struct smbios_type_17 *p = (struct smbios_type_17 *)start;

    p->header.type = 17;
    p->header.length = sizeof(struct smbios_type_17);
    p->header.handle = 0x1100 + instance;

    p->physical_memory_array_handle = 0x1000;
    p->memory_error_information_handle = 0xfffe; /* none provided */
    p->total_width = 64;
    p->data_width = 64;
/* TODO: should assert in case something is wrong   ASSERT((memory_size_mb & ~0x7fff) == 0); */
    p->size = memory_size_mb;
    p->form_factor = 0x09; /* DIMM */
    p->device_set = 0;
    p->device_locator_str = 1;
    p->bank_locator_str = 0;
    p->memory_type = 0x07; /* RAM */
    p->type_detail = 0;

    start += sizeof(struct smbios_type_17);
    snprintf(start, 8, "DIMM %d", instance);
    start += strlen(start) + 1;
    *((uint8_t *)start) = 0;

    return start+1;
}

/* Type 19 -- Memory Array Mapped Address */
static void *
smbios_type_19_init(void *start, uint32_t memory_size_mb, int instance)
{
    struct smbios_type_19 *p = (struct smbios_type_19 *)start;

    p->header.type = 19;
    p->header.length = sizeof(struct smbios_type_19);
    p->header.handle = 0x1300 + instance;

    p->starting_address = instance << 24;
    p->ending_address = p->starting_address + (memory_size_mb << 10) - 1;
    p->memory_array_handle = 0x1000;
    p->partition_width = 1;

    start += sizeof(struct smbios_type_19);
    *((uint16_t *)start) = 0;

    return start + 2;
}

/* Type 20 -- Memory Device Mapped Address */
static void *
smbios_type_20_init(void *start, uint32_t memory_size_mb, int instance)
{
    struct smbios_type_20 *p = (struct smbios_type_20 *)start;

    p->header.type = 20;
    p->header.length = sizeof(struct smbios_type_20);
    p->header.handle = 0x1400 + instance;

    p->starting_address = instance << 24;
    p->ending_address = p->starting_address + (memory_size_mb << 10) - 1;
    p->memory_device_handle = 0x1100 + instance;
    p->memory_array_mapped_address_handle = 0x1300 + instance;
    p->partition_row_position = 1;
    p->interleave_position = 0;
    p->interleaved_data_depth = 0;

    start += sizeof(struct smbios_type_20);

    *((uint16_t *)start) = 0;
    return start+2;
}

/* Type 32 -- System Boot Information */
static void *
smbios_type_32_init(void *start)
{
    struct smbios_type_32 *p = (struct smbios_type_32 *)start;

    p->header.type = 32;
    p->header.length = sizeof(struct smbios_type_32);
    p->header.handle = 0x2000;
    memset(p->reserved, 0, 6);
    p->boot_status = 0; /* no errors detected */

    start += sizeof(struct smbios_type_32);
    *((uint16_t *)start) = 0;

    return start+2;
}

/* Type 127 -- End of Table */
static void *
smbios_type_127_init(void *start)
{
    struct smbios_type_127 *p = (struct smbios_type_127 *)start;

    p->header.type = 127;
    p->header.length = sizeof(struct smbios_type_127);
    p->header.handle = 0x7f00;

    start += sizeof(struct smbios_type_127);
    *((uint16_t *)start) = 0;

    return start + 2;
}

void smbios_init(void)
{
    unsigned cpu_num, nr_structs = 0, max_struct_size = 0;
    char *start, *p, *q;
    int memsize = (ram_end == ram_size) ? ram_size / (1024 * 1024) :
                  (ram_end - (1ull << 32) + ram_size) / (1024 * 1024);
    int i, nr_mem_devs;

    bios_table_cur_addr = align(bios_table_cur_addr, 16);
    start = (void *)(bios_table_cur_addr);

	p = (char *)start + sizeof(struct smbios_entry_point);

#define add_struct(fn) do { \
    q = (fn); \
    nr_structs++; \
    if ((q - p) > max_struct_size) \
        max_struct_size = q - p; \
    p = q; \
} while (0)

    add_struct(smbios_type_0_init(p));
    add_struct(smbios_type_1_init(p));
    add_struct(smbios_type_3_init(p));
    for (cpu_num = 1; cpu_num <= smp_cpus; cpu_num++)
        add_struct(smbios_type_4_init(p, cpu_num));

    /* Each 'memory device' covers up to 16GB of address space. */
    nr_mem_devs = (memsize + 0x3fff) >> 14;
    add_struct(smbios_type_16_init(p, memsize, nr_mem_devs));
    for ( i = 0; i < nr_mem_devs; i++ )
    {
        uint32_t dev_memsize = ((i == (nr_mem_devs - 1))
                                ? (((memsize - 1) & 0x3fff) + 1) : 0x4000);
        add_struct(smbios_type_17_init(p, dev_memsize, i));
        add_struct(smbios_type_19_init(p, dev_memsize, i));
        add_struct(smbios_type_20_init(p, dev_memsize, i));
    }

    add_struct(smbios_type_32_init(p));
    add_struct(smbios_type_127_init(p));

#undef add_struct

    smbios_entry_point_init(
        start, max_struct_size,
        (p - (char *)start) - sizeof(struct smbios_entry_point),
        (uint32_t)(start + sizeof(struct smbios_entry_point)),
        nr_structs);

    bios_table_cur_addr += (p - (char *)start);

    BX_INFO("SMBIOS table addr=0x%08lx\n", (unsigned long)start);
}

static uint32_t find_resume_vector(void)
{
    unsigned long addr, start, end;

#ifdef BX_USE_EBDA_TABLES
    start = align(ebda_cur_addr, 16);
    end = 0xa000 << 4;
#else
    if (bios_table_cur_addr == 0)
        return 0;
    start = align(bios_table_cur_addr, 16);
    end = bios_table_end_addr;
#endif

    for (addr = start; addr < end; addr += 16) {
        if (!memcmp((void*)addr, "RSD PTR ", 8)) {
            struct rsdp_descriptor *rsdp = (void*)addr;
            struct rsdt_descriptor_rev1 *rsdt = (void*)rsdp->rsdt_physical_address;
            struct fadt_descriptor_rev1 *fadt = (void*)rsdt->table_offset_entry[0];
            struct facs_descriptor_rev1 *facs = (void*)fadt->firmware_ctrl;
            return facs->firmware_waking_vector;
        }
    }

    return 0;
}

static void find_440fx(PCIDevice *d)
{
    uint16_t vendor_id, device_id;

    vendor_id = pci_config_readw(d, PCI_VENDOR_ID);
    device_id = pci_config_readw(d, PCI_DEVICE_ID);

    if (vendor_id == PCI_VENDOR_ID_INTEL &&
       (device_id == PCI_DEVICE_ID_INTEL_82441 || device_id == PCI_DEVICE_ID_INTEL_82437))
        i440_pcidev = *d;
}

static void reinit_piix4_pm(PCIDevice *d)
{
    uint16_t vendor_id, device_id;

    vendor_id = pci_config_readw(d, PCI_VENDOR_ID);
    device_id = pci_config_readw(d, PCI_DEVICE_ID);

    if (vendor_id == PCI_VENDOR_ID_INTEL && device_id == PCI_DEVICE_ID_INTEL_82371AB_3)
        piix4_pm_enable(d);
}

void rombios32_init(uint32_t *s3_resume_vector, uint8_t *shutdown_flag)
{
    BX_INFO("Starting rombios32\n");
    BX_INFO("Shutdown flag %x\n", *shutdown_flag);

#ifdef BX_QEMU
    qemu_cfg_port = qemu_cfg_port_probe();
#endif

    ram_probe();

    cpu_probe();

    setup_mtrr();

    smp_probe();

    find_bios_table_area();

    if (*shutdown_flag == 0xfe) {
        /* redirect bios read access to RAM */
        pci_for_each_device(find_440fx);
        bios_lock_shadow_ram(); /* bios is already copied */
        *s3_resume_vector = find_resume_vector();
        if (!*s3_resume_vector) {
            BX_INFO("This is S3 resume but wakeup vector is NULL\n");
        } else {
            BX_INFO("S3 resume vector %p\n", *s3_resume_vector);
            pci_for_each_device(reinit_piix4_pm);
        }
        return;
    }

    pci_bios_init();

#ifndef BX_USE_EBDA_TABLES
    if (bios_table_cur_addr != 0 && i440_pcidev.bus != -1) {

        mptable_init();

        uuid_probe();

        smbios_init();

        if (acpi_enabled)
            acpi_bios_init();

        bios_lock_shadow_ram();

    }
#else
    mptable_init();

    if (bios_table_cur_addr != 0 && i440_pcidev.bus != -1) {

        uuid_probe();

        smbios_init();
    }

    if (acpi_enabled)
        acpi_bios_init();

    BX_INFO("ebda_cur_addr: 0x%08lx\n", ebda_cur_addr);
    if (ebda_cur_addr > 0xA0000)
        BX_PANIC("ebda_cur_addr overflow!\n");
#endif
    BX_INFO("bios_table_cur_addr: 0x%08lx\n", bios_table_cur_addr);
    if (bios_table_cur_addr > bios_table_end_addr)
        BX_PANIC("bios_table_end_addr overflow!\n");
}
