/////////////////////////////////////////////////////////////////////////
// SMBIOS table generation for UEFI/OVMF support
// Copyright (C) 2026 The Bochs Project
//
// Identifies the guest as a real Mac model to satisfy macOS's boot.efi
// and kernel hardware checks. Reference model: iMac15,1 (Retina 5K, Late
// 2014) -- chosen to match the Haswell desktop CPU profile already
// available in cpu/cpudb/intel, and because it's within Mojave's (10.14)
// officially-supported hardware list.
//
// Manufacturer/Product/Family strings below are just the model's public
// name, not sensitive data. The Board-ID in type 2 ("Mac-..." string) is
// what boot.efi and the kernel actually key model-specific behavior off
// of (not type 1's product name, which is only ever shown to the user);
// it's community-documented for this model, not something verified
// against Apple directly -- double check it if boot behavior looks
// model-mismatched. Serial number and UUID are deliberately NOT real
// hardware identifiers: UUID is left at the SMBIOS spec's own "not
// present" sentinel (all-zero), and serial number is a configurable
// placeholder (see smbios_options_parser), same spirit as how the Apple
// SMC's OSK key is handled -- these are per-machine identifiers that
// shouldn't be invented and hardcoded.
/////////////////////////////////////////////////////////////////////////

#define BX_PLUGGABLE

#include "bochs.h"
#include "smbios_tables.h"
#include <cstddef>
#include <cstring>

#define SMBIOS_TABLES_CAPACITY 4096

static const char *smbios_manufacturer = "Apple Inc.";
static const char *smbios_product_name = "iMac15,1";
static const char *smbios_family       = "iMac";
static const char *smbios_board_id     = "Mac-42FD25EABCABB274";
static char smbios_serial_number[64]   = "Bochs000000000";

void smbios_set_serial_number(const char *serial)
{
    strncpy(smbios_serial_number, serial, sizeof(smbios_serial_number) - 1);
    smbios_serial_number[sizeof(smbios_serial_number) - 1] = 0;
}

bx_smbios_tables_c::bx_smbios_tables_c()
{
    tables_blob = NULL;
    tables_size = 0;
    tables_capacity = 0;

    anchor_blob = NULL;
    anchor_size = 0;

    ram_size = 0;
    memory_array_handle = 0;
    next_handle = 0;
    structure_count = 0;
    max_structure_size = 0;
}

bx_smbios_tables_c::~bx_smbios_tables_c()
{
    if (tables_blob) delete[] tables_blob;
    if (anchor_blob) delete[] anchor_blob;
}

void bx_smbios_tables_c::append_structure(void *formatted, Bit32u formatted_len,
                                           const char * const *strings)
{
    Bit8u *p = tables_blob + tables_size;

    memcpy(p, formatted, formatted_len);
    p += formatted_len;
    Bit32u total_len = formatted_len;

    unsigned i;
    for (i = 0; strings[i] != NULL; i++) {
        Bit32u slen = (Bit32u) strlen(strings[i]) + 1; // include NUL
        memcpy(p, strings[i], slen);
        p += slen;
        total_len += slen;
    }
    // string table terminator: a single extra NUL if there was at least
    // one string (its own NUL already accounted for above), or a second
    // NUL if there were none at all (formatted section is never followed
    // by just one zero byte per the spec).
    *p++ = 0;
    total_len += 1;
    if (i == 0) {
        *p++ = 0;
        total_len += 1;
    }

    tables_size += total_len;
    structure_count++;
    if (total_len > max_structure_size) max_structure_size = (Bit16u) total_len;
}

void bx_smbios_tables_c::build_type_0(void)
{
    struct smbios_type_0 t;
    memset(&t, 0, sizeof(t));
    t.header.type = 0;
    t.header.length = sizeof(t);
    t.header.handle = next_handle++;
    t.vendor_str = 1;
    t.bios_version_str = 2;
    t.bios_starting_address_segment = 0xE000;
    t.bios_release_date_str = 3;
    t.bios_rom_size = 0;
    t.bios_characteristics = 0x08; // "BIOS characteristics not supported" bit

    const char *strings[] = { smbios_manufacturer, "1.0", "01/01/2026", NULL };
    append_structure(&t, sizeof(t), strings);
}

void bx_smbios_tables_c::build_type_1(void)
{
    struct smbios_type_1 t;
    memset(&t, 0, sizeof(t));
    t.header.type = 1;
    t.header.length = sizeof(t);
    t.header.handle = next_handle++;
    t.manufacturer_str = 1;
    t.product_name_str = 2;
    t.version_str = 3;
    t.serial_number_str = 4;
    // UUID left all-zero: the SMBIOS spec reserves this as the "not
    // present" sentinel, which is correct here since we don't have (and
    // shouldn't invent) a real per-machine UUID.
    t.wake_up_type = 0x06; // Power Switch
    t.sku_number_str = 0;
    t.family_str = 5;

    const char *strings[] = {
        smbios_manufacturer, smbios_product_name, "1.0",
        smbios_serial_number, smbios_family, NULL
    };
    append_structure(&t, sizeof(t), strings);
}

void bx_smbios_tables_c::build_type_2(void)
{
    struct smbios_type_2 t;
    memset(&t, 0, sizeof(t));
    t.header.type = 2;
    t.header.length = sizeof(t);
    t.header.handle = next_handle++;
    t.manufacturer_str = 1;
    t.product_str = 2; // Board-ID: what boot.efi/the kernel actually check
    t.version_str = 3;
    t.serial_number_str = 0;
    t.asset_tag_number_str = 0;
    t.feature_flags = 0x01; // board is a hosting board
    t.location_str = 0;
    // next_handle already points past this structure's own handle, and
    // type 3 is built immediately after this one, so it'll get exactly
    // the handle next_handle is holding right now.
    t.chassis_handle = next_handle;
    t.board_type = 0x0A;  // Motherboard (Includes Processor, Memory, and I/O)
    t.contained_element_count = 0;

    const char *strings[] = { smbios_manufacturer, smbios_board_id, "1.0", NULL };
    append_structure(&t, sizeof(t), strings);
}

void bx_smbios_tables_c::build_type_3(void)
{
    struct smbios_type_3 t;
    memset(&t, 0, sizeof(t));
    t.header.type = 3;
    t.header.length = sizeof(t);
    t.header.handle = next_handle++;
    t.manufacturer_str = 1;
    t.type = 0x0D; // All in One, matches the iMac's form factor
    t.version_str = 0;
    t.serial_number_str = 0;
    t.asset_tag_number_str = 0;
    t.boot_up_state = 0x03;      // Safe
    t.power_supply_state = 0x03; // Safe
    t.thermal_state = 0x03;      // Safe
    t.security_status = 0x02;    // Unknown
    t.oem_defined = 0;
    t.height = 0;
    t.number_of_power_cords = 1;
    t.contained_element_count = 0;
    t.contained_element_record_length = 0;
    t.sku_number_str = 0;

    const char *strings[] = { smbios_manufacturer, NULL };
    append_structure(&t, sizeof(t), strings);
}

void bx_smbios_tables_c::build_type_16(void)
{
    struct smbios_type_16 t;
    memset(&t, 0, sizeof(t));
    t.header.type = 16;
    t.header.length = sizeof(t);
    t.header.handle = next_handle++;
    memory_array_handle = t.header.handle;
    t.location = 0x03;         // System board or motherboard
    t.use = 0x03;              // System memory
    t.error_correction = 0x03; // None
    Bit64u capacity_kb = ram_size / 1024;
    if (capacity_kb >= 0x80000000ULL) {
        t.maximum_capacity = 0x80000000;
        t.extended_maximum_capacity = ram_size;
    } else {
        t.maximum_capacity = (Bit32u) capacity_kb;
    }
    t.memory_error_information_handle = 0xFFFE; // not provided
    t.number_of_memory_devices = 1;

    const char *strings[] = { NULL };
    append_structure(&t, sizeof(t), strings);
}

void bx_smbios_tables_c::build_type_17(void)
{
    struct smbios_type_17 t;
    memset(&t, 0, sizeof(t));
    t.header.type = 17;
    t.header.length = sizeof(t);
    t.header.handle = next_handle++;
    t.physical_memory_array_handle = memory_array_handle;
    t.memory_error_information_handle = 0xFFFE; // not provided
    t.total_width = 64;
    t.data_width = 64;
    Bit64u size_mb = ram_size >> 20;
    if (size_mb >= 0x7FFF) {
        t.size = 0x7FFF; // real size is in extended_size
        t.extended_size = (Bit32u) size_mb;
    } else {
        t.size = (Bit16u) size_mb;
    }
    t.form_factor = 0x09; // DIMM
    t.device_set = 0;
    t.device_locator_str = 1;
    t.bank_locator_str = 2;
    t.memory_type = 0x18; // DDR3, matches the iMac15,1
    t.type_detail = 0x0080; // Synchronous
    t.speed = 1600;
    t.manufacturer_str = 3;
    t.serial_number_str = 4;
    t.asset_tag_number_str = 0;
    t.part_number_str = 5;
    t.attributes = 0x01; // Single rank
    t.configured_clock_speed = 1600;

    const char *strings[] = { "DIMM0", "BANK 0", "Bochs", "0", "BochsDIMM", NULL };
    append_structure(&t, sizeof(t), strings);
}

void bx_smbios_tables_c::build_type_127(void)
{
    struct smbios_type_127 t;
    memset(&t, 0, sizeof(t));
    t.header.type = 127;
    t.header.length = sizeof(t);
    t.header.handle = next_handle++;

    const char *strings[] = { NULL };
    append_structure(&t, sizeof(t), strings);
}

void bx_smbios_tables_c::build_anchor(void)
{
    anchor_blob = new Bit8u[sizeof(smbios_21_entry_point)];
    anchor_size = sizeof(smbios_21_entry_point);

    struct smbios_21_entry_point ep;
    memset(&ep, 0, sizeof(ep));
    memcpy(ep.anchor_string, "_SM_", 4);
    memcpy(ep.intermediate_anchor_string, "_DMI_", 5);
    ep.length = sizeof(ep);
    ep.smbios_major_version = 2;
    ep.smbios_minor_version = 8;
    ep.smbios_bcd_revision = 0x28;
    ep.entry_point_revision = 0;

    ep.structure_table_length = (Bit16u) tables_size;
    ep.max_structure_size = max_structure_size;
    ep.number_of_structures = structure_count;

    // Left at 0: OVMF computes the real address and both checksums once
    // it places the tables blob in guest memory (same as QEMU's own
    // fw_cfg-delivered anchor -- see smbios_entry_point_setup()).
    ep.structure_table_address = 0;
    ep.checksum = 0;
    ep.intermediate_checksum = 0;

    memcpy(anchor_blob, &ep, sizeof(ep));
}

void bx_smbios_tables_c::generate_tables(Bit64u guest_ram_size)
{
    ram_size = guest_ram_size;
    tables_capacity = SMBIOS_TABLES_CAPACITY;
    tables_blob = new Bit8u[tables_capacity];
    tables_size = 0;

    build_type_0();
    build_type_1();
    build_type_2();
    build_type_3();
    build_type_16();
    build_type_17();
    build_type_127();

    build_anchor();
}
