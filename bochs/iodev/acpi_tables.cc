/////////////////////////////////////////////////////////////////////////
// ACPI table generation for UEFI/OVMF support
// Copyright (C) 2025 The Bochs Project
//
// Uses pre-compiled DSDT from Bochs BIOS (bios/acpi-dsdt.hex)
// which includes proper PCI root bridge, interrupt routing, etc.
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "acpi_tables.h"
#include <cstddef>
#include <cstring>

// Pre-compiled DSDT from Bochs BIOS (bios/acpi-dsdt.hex)
// This includes:
// - PCI Root Bridge (PNP0A03) with _CRS and _PRT
// - Interrupt Link Devices (LNKA-LNKD)
// - ISA Bridge with RTC, keyboard, mouse
// - HPET device
// - S3/S4/S5 sleep states
#include "acpi_dsdt_aml.h"

bx_acpi_tables_c::bx_acpi_tables_c()
{
    tables_blob = NULL;
    tables_size = 0;
    tables_capacity = 0;
    
    rsdp_blob = NULL;
    rsdp_size = 0;
    
    loader_blob = NULL;
    loader_size = 0;
    loader_capacity = 0;
    loader_count = 0;
}

bx_acpi_tables_c::~bx_acpi_tables_c()
{
    if (tables_blob) delete[] tables_blob;
    if (rsdp_blob) delete[] rsdp_blob;
    if (loader_blob) delete[] loader_blob;
}

Bit8u bx_acpi_tables_c::calculate_checksum(Bit8u* data, Bit32u len)
{
    Bit8u sum = 0;
    for (Bit32u i = 0; i < len; i++) {
        sum += data[i];
    }
    return (Bit8u)(0 - sum);
}

// Use pre-compiled DSDT from Bochs BIOS
Bit32u bx_acpi_tables_c::build_dsdt(Bit8u* buffer)
{
    // Copy the pre-compiled DSDT
    memcpy(buffer, AmlCode, sizeof(AmlCode));
    return sizeof(AmlCode);
}

Bit32u bx_acpi_tables_c::build_facs(Bit8u* buffer)
{
    struct acpi_facs* facs = (struct acpi_facs*)buffer;
    memset(facs, 0, sizeof(struct acpi_facs));
    
    memcpy(facs->signature, "FACS", 4);
    facs->length = 64;  // Minimum FACS size
    facs->version = 2;  // ACPI 2.0
    
    return 64;
}

Bit32u bx_acpi_tables_c::build_fadt(Bit8u* buffer, Bit32u facs_offset, Bit32u dsdt_offset)
{
    struct acpi_fadt* fadt = (struct acpi_fadt*)buffer;
    memset(fadt, 0, sizeof(struct acpi_fadt));
    
    // Header
    memcpy(fadt->header.signature, "FACP", 4);
    fadt->header.length = sizeof(struct acpi_fadt);
    fadt->header.revision = 5;  // ACPI 5.0 (good compatibility)
    memcpy(fadt->header.oem_id, ACPI_OEM_ID, 6);
    memcpy(fadt->header.oem_table_id, ACPI_OEM_TABLE_ID, 8);
    fadt->header.oem_revision = 1;
    memcpy(fadt->header.asl_compiler_id, ACPI_ASL_COMPILER_ID, 4);
    fadt->header.asl_compiler_revision = 1;
    
    // FACS and DSDT pointers - store actual offsets, loader will convert to addresses
    fadt->firmware_ctrl = facs_offset;
    fadt->dsdt = dsdt_offset;
    
    // Power management profile
    fadt->preferred_pm_profile = 0;  // Unspecified
    
    // SCI interrupt
    fadt->sci_int = 9;
    
    // SMI command port (PIIX4 ACPI)
    fadt->smi_cmd = 0xB2;
    fadt->acpi_enable = 0xF1;
    fadt->acpi_disable = 0xF0;
    
    // PM1 event block (PIIX4 ACPI at 0xB000)
    fadt->pm1a_evt_blk = 0xB000;
    fadt->pm1_evt_len = 4;
    
    // PM1 control block
    fadt->pm1a_cnt_blk = 0xB004;
    fadt->pm1_cnt_len = 2;
    
    // PM timer block
    fadt->pm_tmr_blk = 0xB008;
    fadt->pm_tmr_len = 4;
    
    // GPE0 block
    fadt->gpe0_blk = 0xB020;
    fadt->gpe0_blk_len = 8;
    
    // P_LVL latencies (not supported)
    fadt->p_lvl2_lat = 0xFFFF;
    fadt->p_lvl3_lat = 0xFFFF;
    
    // FADT flags - CRITICAL: Do NOT set HW_REDUCED_ACPI (bit 20) for legacy PIIX4!
    // Setting that bit tells Windows that PM1/GPE registers don't exist.
    fadt->flags = (1 << 0)    // WBINVD supported
                | (1 << 4)    // Proc C1 supported  
                | (1 << 5)    // P_LVL2_UP (uniprocessor only)
                | (1 << 8)    // RTC_S4 supported
                | (1 << 10);  // 32-bit PM timer
    // Note: bit 20 (HW_REDUCED_ACPI) is intentionally NOT set!
    
    // IA-PC boot architecture flags
    fadt->iapc_boot_arch = (1 << 0)   // Legacy devices
                         | (1 << 1);  // 8042 present
    
    // Extended addresses (64-bit versions) - also store offsets
    fadt->x_firmware_ctrl = facs_offset;
    fadt->x_dsdt = dsdt_offset;
    
    // Extended PM register addresses (GAS format)
    // PM1a Event Block
    fadt->x_pm1a_evt_blk[0] = 1;     // SystemIO
    fadt->x_pm1a_evt_blk[1] = 32;    // Bit width
    fadt->x_pm1a_evt_blk[2] = 0;     // Bit offset
    fadt->x_pm1a_evt_blk[3] = 2;     // Word access
    *((Bit64u*)&fadt->x_pm1a_evt_blk[4]) = 0xB000;
    
    // PM1a Control Block
    fadt->x_pm1a_cnt_blk[0] = 1;     // SystemIO
    fadt->x_pm1a_cnt_blk[1] = 16;    // Bit width
    fadt->x_pm1a_cnt_blk[2] = 0;
    fadt->x_pm1a_cnt_blk[3] = 2;     // Word access
    *((Bit64u*)&fadt->x_pm1a_cnt_blk[4]) = 0xB004;
    
    // PM Timer Block
    fadt->x_pm_tmr_blk[0] = 1;       // SystemIO
    fadt->x_pm_tmr_blk[1] = 32;      // Bit width
    fadt->x_pm_tmr_blk[2] = 0;
    fadt->x_pm_tmr_blk[3] = 3;       // DWord access
    *((Bit64u*)&fadt->x_pm_tmr_blk[4]) = 0xB008;
    
    // GPE0 Block
    fadt->x_gpe0_blk[0] = 1;         // SystemIO
    fadt->x_gpe0_blk[1] = 64;        // Bit width (8 bytes)
    fadt->x_gpe0_blk[2] = 0;
    fadt->x_gpe0_blk[3] = 1;         // Byte access
    *((Bit64u*)&fadt->x_gpe0_blk[4]) = 0xB020;
    
    // Checksum will be recalculated by loader after patching
    fadt->header.checksum = 0;
    
    return sizeof(struct acpi_fadt);
}

Bit32u bx_acpi_tables_c::build_madt(Bit8u* buffer, Bit32u num_cpus)
{
    Bit32u offset = 0;
    
    // MADT header
    struct acpi_madt* madt = (struct acpi_madt*)buffer;
    memset(madt, 0, sizeof(struct acpi_madt));
    
    memcpy(madt->header.signature, "APIC", 4);
    madt->header.revision = 4;  // ACPI 4.0
    memcpy(madt->header.oem_id, ACPI_OEM_ID, 6);
    memcpy(madt->header.oem_table_id, ACPI_OEM_TABLE_ID, 8);
    madt->header.oem_revision = 1;
    memcpy(madt->header.asl_compiler_id, ACPI_ASL_COMPILER_ID, 4);
    madt->header.asl_compiler_revision = 1;
    
    madt->local_apic_address = 0xFEE00000;
    madt->flags = 1;  // PCAT_COMPAT (dual 8259 setup)
    
    offset = sizeof(struct acpi_madt);
    
    // Add Local APIC entries for each CPU
    for (Bit32u i = 0; i < num_cpus; i++) {
        struct acpi_madt_local_apic* lapic = (struct acpi_madt_local_apic*)(buffer + offset);
        lapic->type = ACPI_MADT_TYPE_LOCAL_APIC;
        lapic->length = 8;
        lapic->processor_id = i;
        lapic->apic_id = i;
        lapic->flags = 1;  // Enabled
        offset += 8;
    }
    
    // Add I/O APIC
    struct acpi_madt_io_apic* ioapic = (struct acpi_madt_io_apic*)(buffer + offset);
    ioapic->type = ACPI_MADT_TYPE_IO_APIC;
    ioapic->length = 12;
    ioapic->io_apic_id = num_cpus;  // ID after CPUs
    ioapic->reserved = 0;
    ioapic->address = 0xFEC00000;
    ioapic->global_irq_base = 0;
    offset += 12;
    
    // Add interrupt source override for IRQ0 -> GSI 2 (timer)
    struct acpi_madt_interrupt_override* iso = (struct acpi_madt_interrupt_override*)(buffer + offset);
    iso->type = ACPI_MADT_TYPE_INTERRUPT_OVERRIDE;
    iso->length = 10;
    iso->bus = 0;      // ISA
    iso->source = 0;   // IRQ0
    iso->global_irq = 2;  // Mapped to GSI 2
    iso->flags = 0;    // Conforms to bus spec
    offset += 10;
    
    // Add interrupt source override for IRQ9 (SCI) - level triggered, active low
    iso = (struct acpi_madt_interrupt_override*)(buffer + offset);
    iso->type = ACPI_MADT_TYPE_INTERRUPT_OVERRIDE;
    iso->length = 10;
    iso->bus = 0;      // ISA
    iso->source = 9;   // IRQ9 (SCI)
    iso->global_irq = 9;
    iso->flags = 0x000D;  // Level triggered, active low
    offset += 10;
    
    // Add Local APIC NMI (LINT1)
    struct acpi_madt_local_apic_nmi* nmi = (struct acpi_madt_local_apic_nmi*)(buffer + offset);
    nmi->type = ACPI_MADT_TYPE_LOCAL_APIC_NMI;
    nmi->length = 6;
    nmi->processor_id = 0xFF;  // All processors
    nmi->flags = 0;
    nmi->lint = 1;  // LINT1
    offset += 6;
    
    // Update header length and checksum
    madt->header.length = offset;
    madt->header.checksum = calculate_checksum(buffer, offset);
    
    return offset;
}

Bit32u bx_acpi_tables_c::build_hpet(Bit8u* buffer)
{
    struct acpi_hpet* hpet = (struct acpi_hpet*)buffer;
    memset(hpet, 0, sizeof(struct acpi_hpet));
    
    memcpy(hpet->header.signature, "HPET", 4);
    hpet->header.length = sizeof(struct acpi_hpet);
    hpet->header.revision = 1;
    memcpy(hpet->header.oem_id, ACPI_OEM_ID, 6);
    memcpy(hpet->header.oem_table_id, ACPI_OEM_TABLE_ID, 8);
    hpet->header.oem_revision = 1;
    memcpy(hpet->header.asl_compiler_id, ACPI_ASL_COMPILER_ID, 4);
    hpet->header.asl_compiler_revision = 1;
    
    hpet->event_timer_block_id = 0x8086A201;  // Intel
    
    // Generic Address Structure for base address
    hpet->base_address[0] = 0;     // Address space (memory)
    hpet->base_address[1] = 64;    // Register bit width
    hpet->base_address[2] = 0;     // Register bit offset
    hpet->base_address[3] = 0;     // Access size
    // 64-bit address at offset 4
    Bit64u addr = 0xFED00000ULL;
    memcpy(&hpet->base_address[4], &addr, 8);
    
    hpet->hpet_number = 0;
    hpet->min_tick = 100;
    hpet->page_prot = 0;
    
    hpet->header.checksum = calculate_checksum(buffer, sizeof(struct acpi_hpet));
    
    return sizeof(struct acpi_hpet);
}

Bit32u bx_acpi_tables_c::build_xsdt(Bit8u* buffer, Bit32u* table_offsets, Bit32u num_tables)
{
    Bit32u header_size = sizeof(struct acpi_table_header);
    Bit32u total_size = header_size + num_tables * 8;  // 8 bytes per entry (64-bit pointers)
    
    struct acpi_table_header* xsdt = (struct acpi_table_header*)buffer;
    memset(xsdt, 0, total_size);
    
    memcpy(xsdt->signature, "XSDT", 4);
    xsdt->length = total_size;
    xsdt->revision = 1;
    memcpy(xsdt->oem_id, ACPI_OEM_ID, 6);
    memcpy(xsdt->oem_table_id, ACPI_OEM_TABLE_ID, 8);
    xsdt->oem_revision = 1;
    memcpy(xsdt->asl_compiler_id, ACPI_ASL_COMPILER_ID, 4);
    xsdt->asl_compiler_revision = 1;
    
    // CRITICAL FIX: Actually write the table offsets into the XSDT entries!
    // The loader will add the base address to convert these to physical addresses.
    Bit64u* entries = (Bit64u*)(buffer + header_size);
    for (Bit32u i = 0; i < num_tables; i++) {
        entries[i] = table_offsets[i];
    }
    
    // Checksum will be recalculated by loader after patching
    xsdt->checksum = 0;
    
    return total_size;
}

void bx_acpi_tables_c::build_rsdp(Bit32u xsdt_offset)
{
    rsdp_size = sizeof(struct acpi_rsdp);
    rsdp_blob = new Bit8u[rsdp_size];
    
    struct acpi_rsdp* rsdp = (struct acpi_rsdp*)rsdp_blob;
    memset(rsdp, 0, rsdp_size);
    
    memcpy(rsdp->signature, ACPI_SIG_RSDP, 8);
    memcpy(rsdp->oem_id, ACPI_OEM_ID, 6);
    rsdp->revision = 2;  // ACPI 2.0+
    rsdp->length = 36;   // ACPI 2.0 RSDP size
    
    // RSDT address will be 0 (not provided, only XSDT)
    rsdp->rsdt_address = 0;
    
    // XSDT address - store offset, will be patched by loader
    rsdp->xsdt_address = xsdt_offset;
    
    // Checksums will be recalculated after patching
    rsdp->checksum = 0;
    rsdp->extended_checksum = 0;
}

void bx_acpi_tables_c::add_loader_allocate(const char* file, Bit32u alignment, Bit8u zone)
{
    if (loader_count * sizeof(struct qemu_loader_entry) >= loader_capacity) {
        Bit32u new_capacity = loader_capacity + 16 * sizeof(struct qemu_loader_entry);
        Bit8u* new_blob = new Bit8u[new_capacity];
        if (loader_blob) {
            memcpy(new_blob, loader_blob, loader_size);
            delete[] loader_blob;
        }
        loader_blob = new_blob;
        loader_capacity = new_capacity;
    }
    
    struct qemu_loader_entry* entry = (struct qemu_loader_entry*)(loader_blob + loader_size);
    memset(entry, 0, sizeof(struct qemu_loader_entry));
    
    entry->type = QEMU_LOADER_CMD_ALLOCATE;
    strncpy(entry->command.allocate.file, file, QEMU_LOADER_FNAME_SIZE - 1);
    entry->command.allocate.alignment = alignment;
    entry->command.allocate.zone = zone;
    
    loader_size += sizeof(struct qemu_loader_entry);
    loader_count++;
}

void bx_acpi_tables_c::add_loader_add_pointer(const char* pointer_file, const char* pointee_file,
                                              Bit32u pointer_offset, Bit8u pointer_size)
{
    if (loader_count * sizeof(struct qemu_loader_entry) >= loader_capacity) {
        Bit32u new_capacity = loader_capacity + 16 * sizeof(struct qemu_loader_entry);
        Bit8u* new_blob = new Bit8u[new_capacity];
        if (loader_blob) {
            memcpy(new_blob, loader_blob, loader_size);
            delete[] loader_blob;
        }
        loader_blob = new_blob;
        loader_capacity = new_capacity;
    }
    
    struct qemu_loader_entry* entry = (struct qemu_loader_entry*)(loader_blob + loader_size);
    memset(entry, 0, sizeof(struct qemu_loader_entry));
    
    entry->type = QEMU_LOADER_CMD_ADD_POINTER;
    strncpy(entry->command.add_pointer.pointer_file, pointer_file, QEMU_LOADER_FNAME_SIZE - 1);
    strncpy(entry->command.add_pointer.pointee_file, pointee_file, QEMU_LOADER_FNAME_SIZE - 1);
    entry->command.add_pointer.pointer_offset = pointer_offset;
    entry->command.add_pointer.pointer_size = pointer_size;
    
    loader_size += sizeof(struct qemu_loader_entry);
    loader_count++;
}

void bx_acpi_tables_c::add_loader_add_checksum(const char* file, Bit32u result_offset,
                                               Bit32u start, Bit32u length)
{
    if (loader_count * sizeof(struct qemu_loader_entry) >= loader_capacity) {
        Bit32u new_capacity = loader_capacity + 16 * sizeof(struct qemu_loader_entry);
        Bit8u* new_blob = new Bit8u[new_capacity];
        if (loader_blob) {
            memcpy(new_blob, loader_blob, loader_size);
            delete[] loader_blob;
        }
        loader_blob = new_blob;
        loader_capacity = new_capacity;
    }
    
    struct qemu_loader_entry* entry = (struct qemu_loader_entry*)(loader_blob + loader_size);
    memset(entry, 0, sizeof(struct qemu_loader_entry));
    
    entry->type = QEMU_LOADER_CMD_ADD_CHECKSUM;
    strncpy(entry->command.add_checksum.file, file, QEMU_LOADER_FNAME_SIZE - 1);
    entry->command.add_checksum.result_offset = result_offset;
    entry->command.add_checksum.start = start;
    entry->command.add_checksum.length = length;
    
    loader_size += sizeof(struct qemu_loader_entry);
    loader_count++;
}

void bx_acpi_tables_c::generate_tables(Bit64u ram_size, Bit32u num_cpus)
{
    // Allocate space for tables
    tables_capacity = 64 * 1024;  // 64KB should be plenty
    tables_blob = new Bit8u[tables_capacity];
    memset(tables_blob, 0, tables_capacity);
    
    // Build tables in the blob, tracking offsets
    Bit32u offset = 0;
    
    // 1. DSDT (Definition Block - required) - using pre-compiled from Bochs BIOS
    Bit32u dsdt_offset = offset;
    Bit32u dsdt_size = build_dsdt(tables_blob + offset);
    offset += dsdt_size;
    offset = (offset + 7) & ~7;  // Align to 8 bytes
    
    // 2. FACS (Firmware ACPI Control Structure)
    Bit32u facs_offset = offset;
    Bit32u facs_size = build_facs(tables_blob + offset);
    offset += facs_size;
    offset = (offset + 7) & ~7;
    
    // 3. FADT (Fixed ACPI Description Table)
    Bit32u fadt_offset = offset;
    Bit32u fadt_size = build_fadt(tables_blob + offset, facs_offset, dsdt_offset);
    offset += fadt_size;
    offset = (offset + 7) & ~7;
    
    // 4. MADT (Multiple APIC Description Table)
    Bit32u madt_offset = offset;
    Bit32u madt_size = build_madt(tables_blob + offset, num_cpus);
    offset += madt_size;
    offset = (offset + 7) & ~7;
    
    // 5. HPET table
    Bit32u hpet_offset = offset;
    Bit32u hpet_size = build_hpet(tables_blob + offset);
    offset += hpet_size;
    offset = (offset + 7) & ~7;
    
    // 6. XSDT (Extended System Description Table)
    // Contains pointers to FADT, MADT, HPET (not DSDT, that's referenced in FADT)
    Bit32u table_offsets[] = { fadt_offset, madt_offset, hpet_offset };
    Bit32u num_tables = 3;
    Bit32u xsdt_offset = offset;
    Bit32u xsdt_size = build_xsdt(tables_blob + offset, table_offsets, num_tables);
    offset += xsdt_size;
    offset = (offset + 7) & ~7;
    
    tables_size = offset;
    
    // 7. Build RSDP (goes in separate blob)
    build_rsdp(xsdt_offset);
    
    // 8. Build loader commands
    loader_capacity = 32 * sizeof(struct qemu_loader_entry);
    loader_blob = new Bit8u[loader_capacity];
    memset(loader_blob, 0, loader_capacity);
    loader_size = 0;
    loader_count = 0;
    
    // Allocate the tables blob in high memory
    add_loader_allocate("etc/acpi/tables", 64, QEMU_LOADER_ALLOC_HIGH);
    
    // Allocate the RSDP in FSEG (below 1MB, for BIOS compatibility)
    add_loader_allocate("etc/acpi/rsdp", 16, QEMU_LOADER_ALLOC_FSEG);
    
    // Patch FADT.firmware_ctrl (32-bit) to point to FACS
    add_loader_add_pointer("etc/acpi/tables", "etc/acpi/tables",
                          fadt_offset + offsetof(struct acpi_fadt, firmware_ctrl), 4);
    
    // Patch FADT.dsdt (32-bit) to point to DSDT
    add_loader_add_pointer("etc/acpi/tables", "etc/acpi/tables",
                          fadt_offset + offsetof(struct acpi_fadt, dsdt), 4);
    
    // Patch FADT.x_firmware_ctrl (64-bit) to point to FACS
    add_loader_add_pointer("etc/acpi/tables", "etc/acpi/tables",
                          fadt_offset + offsetof(struct acpi_fadt, x_firmware_ctrl), 8);
    
    // Patch FADT.x_dsdt (64-bit) to point to DSDT
    add_loader_add_pointer("etc/acpi/tables", "etc/acpi/tables",
                          fadt_offset + offsetof(struct acpi_fadt, x_dsdt), 8);
    
    // Patch XSDT entries to point to tables
    Bit32u xsdt_entries_offset = xsdt_offset + sizeof(struct acpi_table_header);
    for (Bit32u i = 0; i < num_tables; i++) {
        add_loader_add_pointer("etc/acpi/tables", "etc/acpi/tables",
                              xsdt_entries_offset + i * 8, 8);
    }
    
    // Patch RSDP.xsdt_address to point to XSDT
    add_loader_add_pointer("etc/acpi/rsdp", "etc/acpi/tables",
                          offsetof(struct acpi_rsdp, xsdt_address), 8);
    
    // Add checksums - these must be recalculated after pointer patching
    
    // FADT checksum
    add_loader_add_checksum("etc/acpi/tables", 
                           fadt_offset + offsetof(struct acpi_table_header, checksum),
                           fadt_offset, fadt_size);
    
    // XSDT checksum
    add_loader_add_checksum("etc/acpi/tables",
                           xsdt_offset + offsetof(struct acpi_table_header, checksum),
                           xsdt_offset, xsdt_size);
    
    // RSDP checksums (two separate checksums in RSDP)
    add_loader_add_checksum("etc/acpi/rsdp", 
                           offsetof(struct acpi_rsdp, checksum), 
                           0, 20);
    add_loader_add_checksum("etc/acpi/rsdp",
                           offsetof(struct acpi_rsdp, extended_checksum),
                           0, 36);
}
