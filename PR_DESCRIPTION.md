# Title

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