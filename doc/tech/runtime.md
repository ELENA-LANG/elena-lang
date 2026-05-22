# ELENA Runtime Architecture

How the ELENA runtime is structured at the level the JIT and the opcode
templates depend on. This is the context required to read the per-arch
JIT bodies in `doc/tech/opcodes/` -- heap layout, object header, frame
model, GC contracts, threading, and external-call ABIs.

The companion documents are:
- [`opcodes/README.md`](opcodes/README.md) -- opcode reference + per-opcode files
- [`linker.md`](linker.md) -- VMT layout, message encoding, reference masks
- [`jit-passes.md`](jit-passes.md) -- bytecode-tape optimizations

## Runtime backends

Two parallel JIT runtimes coexist on x86; the build selects one. They expose
the same opcode surface but differ in concurrency model. **aarch64** and
**ppc64le** currently ship only a single-thread backend.

| Arch | STA file | MTA / GCXT file |
|---|---|---|
| amd64 | `asm/amd64/core60.asm` | `asm/amd64/corex60.asm` |
| x32 | `asm/x32/core60.asm` | `asm/x32/corex60.asm` |
| aarch64 | `asm/aarch64/core60.asm` | -- (not yet implemented) |
| ppc64le | `asm/ppc64le/core60.asm` | -- (not yet implemented) |

The MTA file **overrides** the implementation of 14 opcodes for thread safety:
`snop`, `throw`, `unhook`, `exclude`, `include`, `tststck`, `trylock`,
`freelock`, `peektls`, `storetls`, `extclose`, `system`, `xhookdpr`, `extopen`.
All other opcodes share the body from `core60.asm`. The per-opcode files in
`opcodes/` document both variants when they differ -- look for the
`## MT variant (corex60.asm / GCXT)` section.

## Runtime helper routines

These are not opcodes -- they're predefined references (`GC_ALLOC = 0x10002`,
`VEH_HANDLER = 0x10003`, etc.) whose addresses are linked into the JIT image.
Opcode templates `call` them when they need allocation, GC, or exception
unwinding.

| Symbol | In/Out | Purpose |
|---|---|---|
| `GC_ALLOC` | in `rcx`=size, out `rbx`=new object | Bump-allocate from young generation; on overflow falls through to `GC_COLLECT` |
| `GC_COLLECT` | in `rcx`=requested size, `rdx`=0 minor / 1 full | Walk static roots + perm + every managed frame, build root list, call host `$rt.CollectGCLA` |
| `GC_ALLOCPERM` | in `rcx`=size, out `rbx`=new object | Allocate in PERM (rarely-collected old gen); falls through to `$rt.CollectPermGCLA` |
| `PREPARE` | in `rax`=rsp | First-time runtime init -- calls `$rt.PrepareLA` (Linux/FreeBSD); on Windows is a no-op |
| `THREAD_WAIT` | in `rdx`=collector event handle | Park this thread at a safe point until the collecting thread signals GC end |
| `VEH_HANDLER` | OS-supplied | Vectored exception handler installed at startup; reads `CORE_SINGLE_CONTENT`'s first slot and jumps to the ELENA-side dispatcher |

In `corex60.asm`, `GC_ALLOC` / `GC_COLLECT` / `GC_ALLOCPERM` are wrapped by a
global spinlock acquired via `lock cmpxchg` on `gc_lock`, and `GC_COLLECT`
performs **stop-the-world** synchronization across all threads (see GCXT
section below).

## Heap layout

```
+----------------------------------------------------------+
| Header  |          YG          |           OG            |
+----------------------------------------------------------+
          |   Main   |  Shadow   |  PERM   |     MG        |
+----------------------------------------------------------+
```

- **Header** -- `CORE_GC_TABLE` (bookkeeping pointers: `gc_yg_start`,
  `gc_yg_current`, `gc_yg_end`, `gc_mg_*`, `gc_perm_*`, `gc_lock`,
  `gc_signal`).
- **YG (Young Generation)** -- bump-allocated, frequently collected. `Main` is
  the live half; `Shadow` is the copy target during a minor collect (Cheney
  scavenger style).
- **OG (Old Generation)**
  - **PERM** -- long-lived / promoted objects rarely collected; allocated via
    `GC_ALLOCPERM`. Holds e.g. VMTs.
  - **MG (Mature Generation)** -- surviving promotions from YG. A write-barrier
    page bitmap (`gc_mg_wbar`) marks cards touched since the last collect; the
    `assign` (0x12) and `assigni` (0xA6) opcodes flip those bits.

## Object header layout

```
                       acc (rbx) points here
                              v
[ lock(1B) | flag+length(3B) | size(4B) | VMT_ptr(8B) || field0 | field1 | ... ]
   acc-9      acc-8..acc-5     acc-4..acc-1   acc-0          acc+0   acc+8
```

- `[acc - elSyncOffset]` = `[acc - 8]` -> per-object lock byte (used by
  `trylock` / `freelock` in MT mode).
- `[acc - elSizeOffset]` = `[acc - 4]` -> 4-byte `flag|length` word (bit 30 =
  `struct_mask`, bits 0..29 = size in bytes / 8 => `len` opcode shifts right by 3).
- `[acc - elVMTOffset]` = `[acc - 16]` -> VMT pointer (read by `class`, all
  virtual dispatch opcodes).
- `[acc + i*8]` -> field `i` on 64-bit; `[acc + i*4]` on x32.

A VMT itself has `[vmt + elVMTSizeOffset]` = `[vmt + 8]` (number of dispatch
entries), `[vmt + elVMTFlagOffset]` = `[vmt + 0x18]` (class flags -- tested by
`tstflg`), `[vmt + elPackageOffset]` = `[vmt + 0x20]` (package metadata). See
[`linker.md`](linker.md) for the full VMT body layout.

## Frame model

Three frame shapes exist:

1. **`open i, n`** (`OpenIN`, 0xF0) -- normal in-language call. Pushes prev
   `rbp`, allocates `n` bytes unmanaged, installs the frame header
   (`{prev_fp, 0_acc}`) and `i` managed slots (zero-initialised, GC scans them).
2. **`extopen i, n`** (`ExtOpenIN`, 0xF2) -- external (C ABI) entry into ELENA
   code. Same as `open` but with extra prologue: saves all Win64 home-space
   args, pushes all callee-saved (`rsi, rdi, rbx, r12-r15, rbp`), and in MT
   mode links the new frame into the per-thread `tt_stack_frame` chain.
   Closed with `extclose n` (0xCA).
3. **`xopen i, n`** (`XOpenIN`, 0xDE) -- variant for nested external frames.

Stack walking by the GC starts at the current `tt_stack_frame` and walks
`[frame] -> next_frame` until a NULL terminator; the `0_acc` slot inside each
frame is a managed-pointer field the GC may rewrite during a relocate.

## GCXT model (corex60.asm only)

The multi-threaded runtime adds these mechanisms on top of the single-threaded
one.

### TLS access -- `gs:[58h]` chain

`gs:[58h]` is the Windows TIB self-pointer (and an equivalent FS/GS slot on
other supported OSes). One indirection from there yields the per-thread block
(a `CORE_SINGLE_CONTENT`-shaped struct, but one per thread instead of one
global). The pattern appears in nearly every MT opcode body:

```asm
  mov  rcx, gs:[58h]      ; load TIB self-pointer
  mov  rax, [rcx]         ; deref to this thread's block
  ; rax + et_current, tt_stack_frame, tt_sync_event, tt_flags, tt_stack_root, ...
```

Offsets used:
- `et_current` (0x08) -- head of this thread's exception chain
- `tt_stack_frame` (0x10) -- top of this thread's managed-frame chain
- `tt_sync_event` (0x18) -- OS event handle used to park/wake this thread
- `tt_flags` (0x20) -- bit 0 = "in safe region" (set by `exclude`, cleared by
  `include`); a thread with this bit set is skipped by the stop-the-world wait
- `tt_stack_root` (0x28) -- bottom of this thread's stack (used by `tststck`)

### Locks

| Lock | Where | Acquired | Released | Used by |
|---|---|---|---|---|
| `gc_lock` | `CORE_GC_TABLE + 0x70` | `lock cmpxchg dword ptr [gc_lock], 1` spin | `lock xadd [gc_lock], -1` | `GC_ALLOC`, `GC_COLLECT`, `system 6/7` |
| `gc_signal` | `CORE_GC_TABLE + 0x78` | written by collector thread | cleared at GC end | safe-point check; `THREAD_WAIT` |
| per-object | `[acc - elSyncOffset]` | `lock cmpxchg byte ptr [acc-8], 1` | `lock xadd byte ptr [acc-8], -1` | `trylock` (0x2B), `freelock` (0x2C) |

### Safe points

`snop` (0x02) in MT mode is not a true no-op -- it reads `gc_signal` and, if a
collector thread is parked waiting, calls `THREAD_WAIT` so the GC can proceed.
The compiler inserts `snop` at loop back-edges and other long-running spots
where a thread would otherwise starve the collector.

### Stop-the-world

When a thread enters `GC_COLLECT` in MT mode:
1. It acquires `gc_lock`, writes its `tt_sync_event` into `gc_signal` to mark
   itself as the collector.
2. Walks `CORE_THREAD_TABLE.tt_slots[]`; for each other thread not in a safe
   region (`tt_flags` bit 0 clear), pushes that thread's `tt_sync_event` on a
   wait list and clears its event.
3. Releases `gc_lock` and calls `$rt.WaitForSignalsGCLA` to block until all
   listed threads have signalled (they do so by hitting their own safe-point
   in `snop` or any other allocating opcode and calling `THREAD_WAIT`).
4. With the world stopped, walks roots (static + perm + every thread's TLS
   slots + every thread's frame chain), calls `$rt.CollectGCLA`.
5. Clears `gc_signal` and signals every parked thread to resume.

### Thread table

`CORE_THREAD_TABLE` holds a length word followed by an array of
`{tls_entry_ptr, padding}` slots -- one per live thread. `system 3` (n=3)
registers the current thread in this table at the index passed in `index`
(`rdx`) and stores `rsp` as that thread's `tt_stack_root` so the GC knows
where the stack bottom is.

## External call ABI per arch (`CallExtR 0xFE`)

The `n` operand on `CallExtR` is the **argument count**. What the JIT does
with it depends on the platform calling convention:

| Arch / OS | Convention | Use of `n` |
|---|---|---|
| **x32** (any OS) | cdecl | After the `call`, JIT emits `add esp, n*4` to clean up caller-pushed args. |
| **amd64 Linux/FreeBSD** | System V | Args distributed across `rdi, rsi, rdx, rcx, r8, r9` (up to 6); spillover pushed on stack. JIT pre-aligns RSP to 16 before `call`. |
| **amd64 Windows** | Win64 | Args in `rcx, rdx, r8, r9` (up to 4); JIT reserves 32-byte shadow space (`sub rsp, 20h`) before the call. Spillover passed on stack above the shadow. |
| **aarch64** | AAPCS | Args in `x0..x7`. Spillover on stack. No caller cleanup. |
| **ppc64le** | ELFv2 | Args in `r3..r10`. Spillover on stack. Caller saves/restores TOC (`r2`). |

Argument *loading* from ELENA stack slots into the platform-ABI registers is
emitted by the inline variant -- the prefix selected by `loadCallOp` based on
`n` (see [`jit-passes.md`](jit-passes.md)) is what picks the right
"load N args" body.
