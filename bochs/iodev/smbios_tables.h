/////////////////////////////////////////////////////////////////////////
// SMBIOS table structures and generation for UEFI/OVMF support
// Copyright (C) 2026 The Bochs Project
/////////////////////////////////////////////////////////////////////////

// Delivered to OVMF via fw_cfg as "etc/smbios/smbios-tables" and
// "etc/smbios/smbios-anchor", the same two files QEMU's own SMBIOS
// support (hw/i386/fw_cfg.c, hw/smbios/smbios.c) uses -- OVMF already
// knows to look for exactly these names. Structure layouts below are
// taken from the SMBIOS spec via QEMU's include/hw/firmware/smbios.h.
//
// Unlike ACPI tables, SMBIOS delivery needs no table-loader script: the
// anchor's structure_table_address and both checksums are left at 0
// ("BIOS must recalculate" -- see QEMU's smbios_entry_point_setup()),
// since only OVMF knows the final guest-physical address it will place
// the tables blob at.

#ifndef BX_SMBIOS_TABLES_H
#define BX_SMBIOS_TABLES_H

#include "bochs.h"

#pragma pack(push, 1)

// This goes at the beginning of every SMBIOS structure.
struct smbios_structure_header {
    Bit8u type;
    Bit8u length;
    Bit16u handle;
};

// SMBIOS 2.1 (32-bit) Entry Point -- sufficient for our target guests;
// see smbios_type_4_len_ver-style comment in QEMU if a 3.0/64-bit entry
// point is ever needed.
struct smbios_21_entry_point {
    char anchor_string[4];
    Bit8u checksum;
    Bit8u length;
    Bit8u smbios_major_version;
    Bit8u smbios_minor_version;
    Bit16u max_structure_size;
    Bit8u entry_point_revision;
    Bit8u formatted_area[5];
    char intermediate_anchor_string[5];
    Bit8u intermediate_checksum;
    Bit16u structure_table_length;
    Bit32u structure_table_address;
    Bit16u number_of_structures;
    Bit8u smbios_bcd_revision;
};

// SMBIOS type 0 - BIOS Information
struct smbios_type_0 {
    struct smbios_structure_header header;
    Bit8u vendor_str;
    Bit8u bios_version_str;
    Bit16u bios_starting_address_segment;
    Bit8u bios_release_date_str;
    Bit8u bios_rom_size;
    Bit64u bios_characteristics;
    Bit8u bios_characteristics_extension_bytes[2];
    Bit8u system_bios_major_release;
    Bit8u system_bios_minor_release;
    Bit8u embedded_controller_major_release;
    Bit8u embedded_controller_minor_release;
};

struct smbios_uuid {
    Bit32u time_low;
    Bit16u time_mid;
    Bit16u time_hi_and_version;
    Bit8u clock_seq_hi_and_reserved;
    Bit8u clock_seq_low;
    Bit8u node[6];
};

// SMBIOS type 1 - System Information
struct smbios_type_1 {
    struct smbios_structure_header header;
    Bit8u manufacturer_str;
    Bit8u product_name_str;
    Bit8u version_str;
    Bit8u serial_number_str;
    struct smbios_uuid uuid;
    Bit8u wake_up_type;
    Bit8u sku_number_str;
    Bit8u family_str;
};

// SMBIOS type 2 - Base Board. On real Macs the "product" string here is
// the Board-ID ("Mac-xxxxxxxxxxxxxxxx"), which is what boot.efi and the
// kernel actually key model-specific behavior off of -- not type 1's
// product_name_str, which is only ever shown to the user.
struct smbios_type_2 {
    struct smbios_structure_header header;
    Bit8u manufacturer_str;
    Bit8u product_str;
    Bit8u version_str;
    Bit8u serial_number_str;
    Bit8u asset_tag_number_str;
    Bit8u feature_flags;
    Bit8u location_str;
    Bit16u chassis_handle;
    Bit8u board_type;
    Bit8u contained_element_count;
};

// SMBIOS type 3 - System Enclosure
struct smbios_type_3 {
    struct smbios_structure_header header;
    Bit8u manufacturer_str;
    Bit8u type;
    Bit8u version_str;
    Bit8u serial_number_str;
    Bit8u asset_tag_number_str;
    Bit8u boot_up_state;
    Bit8u power_supply_state;
    Bit8u thermal_state;
    Bit8u security_status;
    Bit32u oem_defined;
    Bit8u height;
    Bit8u number_of_power_cords;
    Bit8u contained_element_count;
    Bit8u contained_element_record_length;
    Bit8u sku_number_str;
};

// SMBIOS type 127 - End-of-table
struct smbios_type_127 {
    struct smbios_structure_header header;
};

#pragma pack(pop)

// SMBIOS tables generator class
class bx_smbios_tables_c {
public:
    bx_smbios_tables_c();
    ~bx_smbios_tables_c();

    // Generate the tables + anchor blobs
    void generate_tables(void);

    Bit8u* get_tables_blob() { return tables_blob; }
    Bit32u get_tables_size() { return tables_size; }

    Bit8u* get_anchor_blob() { return anchor_blob; }
    Bit32u get_anchor_size() { return anchor_size; }

private:
    void build_type_0(void);
    void build_type_1(void);
    void build_type_2(void);
    void build_type_3(void);
    void build_type_127(void);
    void build_anchor(void);

    // Append a formatted structure + its trailing string table to
    // tables_blob, advancing tables_size. 'strings' is a NULL-terminated
    // array of C strings (may be empty, i.e. strings[0] == NULL).
    void append_structure(void *formatted, Bit32u formatted_len,
                           const char * const *strings);

    Bit16u next_handle;
    Bit16u structure_count;
    Bit16u max_structure_size;

    Bit8u* tables_blob;
    Bit32u tables_size;
    Bit32u tables_capacity;

    Bit8u* anchor_blob;
    Bit32u anchor_size;
};

// Override the default (placeholder) serial number, e.g. from a bochsrc
// addon option. Truncated to fit the internal buffer if too long.
void smbios_set_serial_number(const char *serial);

#endif // BX_SMBIOS_TABLES_H
