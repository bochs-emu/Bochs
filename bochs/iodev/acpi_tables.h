/////////////////////////////////////////////////////////////////////////
// ACPI table structures and generation for UEFI/OVMF support
// Copyright (C) 2025 The Bochs Project
/////////////////////////////////////////////////////////////////////////

#ifndef BX_ACPI_TABLES_H
#define BX_ACPI_TABLES_H

#include "bochs.h"

// ACPI table signatures
#define ACPI_SIG_RSDP "RSD PTR "
#define ACPI_SIG_RSDT "RSDT"
#define ACPI_SIG_XSDT "XSDT"
#define ACPI_SIG_FADT "FACP"
#define ACPI_SIG_FACS "FACS"
#define ACPI_SIG_DSDT "DSDT"
#define ACPI_SIG_MADT "APIC"
#define ACPI_SIG_HPET "HPET"
#define ACPI_SIG_MCFG "MCFG"

// ACPI OEM strings
#define ACPI_OEM_ID       "BOCHS "
#define ACPI_OEM_TABLE_ID "BXPC    "
#define ACPI_ASL_COMPILER_ID "BXPC"

// QEMU loader command types
#define QEMU_LOADER_CMD_ALLOCATE     1
#define QEMU_LOADER_CMD_ADD_POINTER  2
#define QEMU_LOADER_CMD_ADD_CHECKSUM 3
#define QEMU_LOADER_CMD_WRITE_POINTER 4

// QEMU loader allocation zones
#define QEMU_LOADER_ALLOC_HIGH 1
#define QEMU_LOADER_ALLOC_FSEG 2

// File name size (from QEMU spec)
#define QEMU_LOADER_FNAME_SIZE 56

#pragma pack(push, 1)

// ACPI table header (common to all tables except RSDP)
struct acpi_table_header {
    char signature[4];
    Bit32u length;
    Bit8u revision;
    Bit8u checksum;
    char oem_id[6];
    char oem_table_id[8];
    Bit32u oem_revision;
    char asl_compiler_id[4];
    Bit32u asl_compiler_revision;
};

// RSDP - Root System Description Pointer (ACPI 2.0+)
struct acpi_rsdp {
    char signature[8];         // "RSD PTR "
    Bit8u checksum;            // Checksum of first 20 bytes
    char oem_id[6];
    Bit8u revision;            // 2 for ACPI 2.0+
    Bit32u rsdt_address;       // 32-bit RSDT address
    Bit32u length;             // RSDP length (36 for ACPI 2.0+)
    Bit64u xsdt_address;       // 64-bit XSDT address
    Bit8u extended_checksum;   // Checksum of entire RSDP
    Bit8u reserved[3];
};

// FACS - Firmware ACPI Control Structure
struct acpi_facs {
    char signature[4];         // "FACS"
    Bit32u length;             // 64 minimum
    Bit32u hardware_signature;
    Bit32u firmware_waking_vector;
    Bit32u global_lock;
    Bit32u flags;
    Bit64u x_firmware_waking_vector;
    Bit8u version;
    Bit8u reserved1[3];
    Bit32u ospm_flags;
    Bit8u reserved2[24];
};

// FADT - Fixed ACPI Description Table (ACPI 6.0)
struct acpi_fadt {
    struct acpi_table_header header;
    Bit32u firmware_ctrl;      // FACS address (32-bit)
    Bit32u dsdt;               // DSDT address (32-bit)
    Bit8u reserved1;           // Was INT_MODEL
    Bit8u preferred_pm_profile;
    Bit16u sci_int;
    Bit32u smi_cmd;
    Bit8u acpi_enable;
    Bit8u acpi_disable;
    Bit8u s4bios_req;
    Bit8u pstate_cnt;
    Bit32u pm1a_evt_blk;
    Bit32u pm1b_evt_blk;
    Bit32u pm1a_cnt_blk;
    Bit32u pm1b_cnt_blk;
    Bit32u pm2_cnt_blk;
    Bit32u pm_tmr_blk;
    Bit32u gpe0_blk;
    Bit32u gpe1_blk;
    Bit8u pm1_evt_len;
    Bit8u pm1_cnt_len;
    Bit8u pm2_cnt_len;
    Bit8u pm_tmr_len;
    Bit8u gpe0_blk_len;
    Bit8u gpe1_blk_len;
    Bit8u gpe1_base;
    Bit8u cst_cnt;
    Bit16u p_lvl2_lat;
    Bit16u p_lvl3_lat;
    Bit16u flush_size;
    Bit16u flush_stride;
    Bit8u duty_offset;
    Bit8u duty_width;
    Bit8u day_alrm;
    Bit8u mon_alrm;
    Bit8u century;
    Bit16u iapc_boot_arch;
    Bit8u reserved2;
    Bit32u flags;
    // Generic Address Structure for RESET_REG
    Bit8u reset_reg[12];
    Bit8u reset_value;
    Bit16u arm_boot_arch;
    Bit8u fadt_minor_version;
    // ACPI 2.0+ fields
    Bit64u x_firmware_ctrl;
    Bit64u x_dsdt;
    Bit8u x_pm1a_evt_blk[12];
    Bit8u x_pm1b_evt_blk[12];
    Bit8u x_pm1a_cnt_blk[12];
    Bit8u x_pm1b_cnt_blk[12];
    Bit8u x_pm2_cnt_blk[12];
    Bit8u x_pm_tmr_blk[12];
    Bit8u x_gpe0_blk[12];
    Bit8u x_gpe1_blk[12];
    Bit8u sleep_control_reg[12];
    Bit8u sleep_status_reg[12];
    Bit64u hypervisor_vendor_id;
};

// MADT - Multiple APIC Description Table
struct acpi_madt {
    struct acpi_table_header header;
    Bit32u local_apic_address;
    Bit32u flags;
    // Variable-length entries follow
};

// MADT entry types
#define ACPI_MADT_TYPE_LOCAL_APIC 0
#define ACPI_MADT_TYPE_IO_APIC 1
#define ACPI_MADT_TYPE_INTERRUPT_OVERRIDE 2
#define ACPI_MADT_TYPE_LOCAL_APIC_NMI 4

struct acpi_madt_local_apic {
    Bit8u type;
    Bit8u length;
    Bit8u processor_id;
    Bit8u apic_id;
    Bit32u flags;
};

struct acpi_madt_io_apic {
    Bit8u type;
    Bit8u length;
    Bit8u io_apic_id;
    Bit8u reserved;
    Bit32u address;
    Bit32u global_irq_base;
};

struct acpi_madt_interrupt_override {
    Bit8u type;
    Bit8u length;
    Bit8u bus;
    Bit8u source;
    Bit32u global_irq;
    Bit16u flags;
};

struct acpi_madt_local_apic_nmi {
    Bit8u type;
    Bit8u length;
    Bit8u processor_id;
    Bit16u flags;
    Bit8u lint;
};

// HPET table
struct acpi_hpet {
    struct acpi_table_header header;
    Bit32u event_timer_block_id;
    Bit8u base_address[12];  // Generic Address Structure
    Bit8u hpet_number;
    Bit16u min_tick;
    Bit8u page_prot;
};

// QEMU loader entry (128 bytes)
struct qemu_loader_entry {
    Bit32u type;
    union {
        struct {
            char file[QEMU_LOADER_FNAME_SIZE];
            Bit32u alignment;
            Bit8u zone;
        } allocate;
        struct {
            char pointer_file[QEMU_LOADER_FNAME_SIZE];
            char pointee_file[QEMU_LOADER_FNAME_SIZE];
            Bit32u pointer_offset;
            Bit8u pointer_size;
        } add_pointer;
        struct {
            char file[QEMU_LOADER_FNAME_SIZE];
            Bit32u result_offset;
            Bit32u start;
            Bit32u length;
        } add_checksum;
        struct {
            char pointer_file[QEMU_LOADER_FNAME_SIZE];
            char pointee_file[QEMU_LOADER_FNAME_SIZE];
            Bit32u pointer_offset;
            Bit32u pointee_offset;
            Bit8u pointer_size;
        } write_pointer;
        Bit8u padding[124];
    } command;
};

#pragma pack(pop)

// ACPI tables generator class
class bx_acpi_tables_c {
public:
    bx_acpi_tables_c();
    ~bx_acpi_tables_c();
    
    // Generate all ACPI tables
    void generate_tables(Bit64u ram_size, Bit32u num_cpus);
    
    // Get generated data
    Bit8u* get_tables_blob() { return tables_blob; }
    Bit32u get_tables_size() { return tables_size; }
    
    Bit8u* get_rsdp_blob() { return rsdp_blob; }
    Bit32u get_rsdp_size() { return rsdp_size; }
    
    Bit8u* get_loader_blob() { return loader_blob; }
    Bit32u get_loader_size() { return loader_size; }
    
private:
    // Calculate checksum for ACPI tables
    Bit8u calculate_checksum(Bit8u* data, Bit32u len);
    
    // Build individual tables
    Bit32u build_dsdt(Bit8u* buffer);
    Bit32u build_facs(Bit8u* buffer);
    Bit32u build_fadt(Bit8u* buffer, Bit32u facs_offset, Bit32u dsdt_offset);
    Bit32u build_madt(Bit8u* buffer, Bit32u num_cpus);
    Bit32u build_hpet(Bit8u* buffer);
    Bit32u build_xsdt(Bit8u* buffer, Bit32u* table_offsets, Bit32u num_tables);
    void build_rsdp(Bit32u xsdt_offset);
    
    // Add loader commands
    void add_loader_allocate(const char* file, Bit32u alignment, Bit8u zone);
    void add_loader_add_pointer(const char* pointer_file, const char* pointee_file,
                                Bit32u pointer_offset, Bit8u pointer_size);
    void add_loader_add_checksum(const char* file, Bit32u result_offset,
                                 Bit32u start, Bit32u length);
    
    // Blobs
    Bit8u* tables_blob;
    Bit32u tables_size;
    Bit32u tables_capacity;
    
    Bit8u* rsdp_blob;
    Bit32u rsdp_size;
    
    Bit8u* loader_blob;
    Bit32u loader_size;
    Bit32u loader_capacity;
    Bit32u loader_count;
};

#endif // BX_ACPI_TABLES_H
