# Title

memory_stub: fix write-stamp invalidation for chunked writes

# Summary

This branch fixes `BX_MEMORY_STUB_C::writePhysicalPage()` so the write-stamp invalidation follows the physical address as a write advances through a page.

For write sizes that fall through to the chunked write loop, upstream currently calls `pageWriteStampTable.decWriteStamp()` repeatedly with the original physical address instead of the current write position. That can leave later 128-byte regions marked as executable even after they were overwritten.

# Why

`decWriteStamp(pAddr, len)` invalidates fine-grained icache occupancy for the sub-page region overlapped by the write. In the chunked path, the code already advances `linear_addr` and `data_ptr`, but it never advances the physical address passed to `decWriteStamp()`. As a result, repeated invalidations target the first region only.

# Scope

This branch is intentionally limited to `bochs/memory/memory_stub.cc` inside `writePhysicalPage()`.
It does not include any of the rejected icache hashing/indexing changes from PR #742.

# Suggested Validation

- Perform a write that spans multiple 128-byte regions within a single page.
- Confirm the corresponding translated code cache entries are invalidated across the whole write span rather than only at the first chunk.# Title

main/misc_mem: fix optional RAM image loading for high addresses

# Summary

This branch fixes the optional RAM image load path so it behaves correctly for high guest physical addresses and non-contiguous RAM backing-store layout:

- `main.cc` now passes the configured RAM image address with `get64()` instead of truncating it through `get()`.
- `BX_MEM_C::load_RAM()` now loads page by page, applies `A20ADDR()`, rejects PCI-hole targets, translates guest physical addresses through `bx_translate_gpa_to_linear()`, and uses 64-bit size/accounting.

# Why

Current upstream behavior truncates configured RAM image addresses above 2GB and treats the guest physical address as a direct linear RAM offset during the load. That is incorrect for:

1. High guest RAM image addresses.
2. Systems where the PCI hole makes guest physical memory non-contiguous in the backing store.
3. Large image sizes on hosts where `unsigned long` is too narrow.

# Scope

This branch is intentionally limited to the optional RAM image load path in `bochs/main.cc` and `bochs/memory/misc_mem.cc`.
It does not include any icache or debugger-only changes from PR #742.

# Suggested Validation

- Configure an optional RAM image above 2GB and confirm it loads at the requested address.
- Confirm loading fails cleanly if the configured address points into the PCI hole.
- Confirm multi-page image loads succeed across page boundaries without aliasing into the wrong RAM backing-store offsets.# Title

memory_stub: fix debugger memory access and CRC32 handling

# Summary

This branch fixes three debugger-facing issues in `memory_stub.cc`:

- `dbg_fetch_mem()` now treats PCI-hole addresses as invalid and translates guest physical addresses through `bx_translate_gpa_to_linear()` before touching the RAM backing store.
- `dbg_set_mem()` now validates the whole range up front, rejects PCI-hole and out-of-RAM accesses, and then writes through translated linear addresses.
- `dbg_crc32()` now terminates correctly, accumulates CRC across page-contained chunks using `crc32_extend()`, and rejects PCI-hole / out-of-RAM addresses instead of aliasing them into RAM.

# Why

Current upstream behavior has three correctness problems:

1. Debugger reads and writes can alias PCI-hole addresses into the RAM backing store.
2. Multi-byte debugger reads and writes increment with raw `a20addr++`, which bypasses A20 wrapping semantics.
3. `dbg_crc32()` resets the CRC on every chunk and loops forever because the chunk loop never terminates.

# Scope

This branch is intentionally limited to debugger memory helpers and CRC logic in `bochs/memory/memory_stub.cc`.
It does not include any of the rejected icache hashing/indexing changes from PR #742.

# Suggested Validation

- Run debugger memory reads/writes across the PCI hole and confirm they fail cleanly.
- Run debugger memory reads/writes across the A20 wrap boundary.
- Run the debugger CRC command across a multi-page range and confirm it completes and returns stable output.