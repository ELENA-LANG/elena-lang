# ELENA Bytecode (ecode) -- Opcode Reference

One file per opcode in this directory. Each file follows the template below.
Source of truth is `elenasrc3/engine/bytecode.h` (hex codes), `elenasrc3/engine/bytecode.cpp`
(`_fnOpcodes[]` mnemonic table) and the per-architecture JIT templates under
`asm/<arch>/core60.asm` (with `corex60.asm` overlays where MT is supported).
`.esm` files (elena-assembly) are the human-writable source that emits these
opcodes.

For context that spans the opcode set rather than living per-opcode:
- [`../runtime.md`](../runtime.md) -- heap layout, object header, frame model,
  GCXT/threading, runtime helpers (`GC_ALLOC`, `GC_COLLECT`, `PREPARE`,
  `THREAD_WAIT`, `VEH_HANDLER`), `CallExtR` ABI per arch.
- [`../linker.md`](../linker.md) -- VMT layout, `mssg_t` encoding,
  reference masks (`mskXxx`) and which masks each R-command accepts.
- [`../jit-passes.md`](../jit-passes.md) -- bytecode-tape optimizer,
  `opSetAcc` / `opNotUsingAcc` liveness, jump folding, `ByteCodeTransformer`
  peephole rules, the variant-selection families (`retrieveICode`,
  `retrieveCode`, etc.) and the argument rewrites (`getFPOffset`,
  `stackOffset + arg`, `importMessage`).

Supported architectures:

| Arch | Files | Threading model |
|---|---|---|
| **amd64** | `asm/amd64/core60.asm`, `asm/amd64/corex60.asm` | STA + MTA (GCXT) |
| **x32** (x86-32) | `asm/x32/core60.asm`, `asm/x32/corex60.asm` | STA + MTA (GCXT) |
| **aarch64** (ARM64) | `asm/aarch64/core60.asm` | STA only |
| **ppc64le** (POWER) | `asm/ppc64le/core60.asm` | STA only |

The bytecode itself is identical across all four; only the JIT emission differs.

## Register conventions

The four roles (frame pointer, stack pointer, accumulator, index, two shadow
slots, scratch) map to different physical registers per ABI. Templates assume
these mappings and never re-allocate them.

### amd64 (Linux SysV / Windows x64)

| Role | Register | Notes |
|---|---|---|
| frame pointer `fp` | `rbp` | preserved |
| stack pointer `sp` | `rsp` | only positive offsets |
| accumulator `acc` | `rbx` | preserved across calls |
| index (data acc.) | `rdx` | preserved |
| `sp[0]` shadow | `r10` | preserved |
| `sp[1]` shadow | `r11` | preserved |
| presaved scratch | `r15` | must be preserved |
| scratch | `rax, rcx, rsi, rdi, r8, r9, r12, r13, r14` | free |

### x32 (System V i386 / cdecl)

| Role | Register | Notes |
|---|---|---|
| frame pointer `fp` | `ebp` | preserved |
| stack pointer `sp` | `esp` | only positive offsets |
| accumulator `acc` | `ebx` | preserved |
| index (data acc.) | `edx` | preserved |
| `sp[0]` shadow | `esi` | preserved |
| scratch | `eax, ecx, edi` | free |

No `sp[1]` shadow (single-shadow ABI). Pointers are 4 bytes; `dp[i]` uses
`*4` stride instead of `*8`. Object header offsets are halved (`elVMTOffset=8`,
`elSizeOffset=4`).

### aarch64 (AArch64 AAPCS)

| Role | Register | Notes |
|---|---|---|
| frame pointer `fp` | `x29` | preserved |
| stack pointer `sp` | `sp` (x31) | 16-byte aligned |
| accumulator `acc` | `x10` | preserved |
| index (data acc.) | `x9` | preserved |
| `sp[0]` shadow | `x0` | preserved |
| `sp[1]` shadow | `x1` | preserved |
| scratch | `x11, x12, x13, x14, x15, x16, x17` | free |
| link register | `x30` | clobbered by `bl`/`blr` |

Global addresses are formed with `adrp` + `add` (Linux/FreeBSD use `movz`/`movk`
pairs over 32-bit halves; macOS uses `adrp data_page : %SYM` + `add ... data_pageoff`).
Conditional comparison results live in `NZCV`; opcodes that set COMP use
`cmp`/`subs` and the JIT picks the matching condition (`b.eq`, `b.lt`, ...)
for the paired `jXX` opcode.

### ppc64le (Power ELFv2 LE)

| Role | Register | Notes |
|---|---|---|
| frame pointer `fp` | `r31` | preserved |
| stack pointer `sp` | `r1` | back-chain at `0(r1)` |
| accumulator `acc` | `r15` | preserved |
| index (data acc.) | `r14` | preserved |
| `sp[0]` shadow | `r3` | preserved |
| `sp[1]` shadow | `r4` | preserved |
| TOC pointer | `r2` | preserved; gives access to `CORE_TOC` |
| scratch | `r16, r17, r18, r19, r20, r21, r22` | free |
| LR / CTR | `lr`, `ctr` | branch-and-link targets |

Globals are reached via `CORE_TOC` indirection: `ld r19, toc_gctable(r2)`
loads the GC-table pointer, then offsets are applied to `r19`. Comparison
results live in CR0; `cmpd`/`cmpld`/`cmpdi` set it and conditional branches
use `beq`/`blt`/`bge cr0, ...`.

### Shared abstractions

`dp[i]` is a slot in the current data frame, addressed off the frame pointer
(positive = current frame, negative = previous). `acc[i]` is field `i` of the
object pointed to by the accumulator. `sp[i]` is the `i`-th word above the
stack pointer (the first two are typically held in the shadow registers).

## Categories (by encoding)

| Range | Width | Form |
|---|---|---|
| `0x00 - 0x35`, `0x70 - 0x74` | 1 byte | single-op (no argument) |
| `0x75 - 0xCF` | 1+sizeof(arg) | double-op (one `arg_t` argument) |
| `0xD0 - 0xFE` | 1+2*sizeof(arg) | triple-op (two `arg_t` arguments) |
| `0x1001` (`Label`) | meta | label placeholder, not emitted as code |

Boundaries are defined by `ByteCode::MaxSingleOp = 0x74` and
`ByteCode::MaxDoubleOp = 0xCF` in `bytecode.h`.

## ABI quirks per architecture

| Concern | amd64 | x32 | aarch64 | ppc64le |
|---|---|---|---|---|
| Word size | 8 B | 4 B | 8 B | 8 B |
| `acc[i]` stride | `*8` | `*4` | `*8` | `*8` |
| `elVMTOffset` | 16 | 8 | 16 | 16 |
| `elSizeOffset` | 4 | 4 | 4 | 4 |
| `page_size_order` | 5 (32-B cards) | 4 (16-B cards) | 5 | 5 |
| Global access | direct `[data : %SYM]` | direct `[data : %SYM]` | `adrp`+`add` (Mac) or `movz`+`movk` pair (Lnx/BSD) | TOC-indirect via `ld r?, toc_*(r2)` |
| Comparison flags | EFLAGS | EFLAGS | NZCV (cmp/subs) | CR0 (cmpd/cmpld/cmpdi) |
| `len` shift | `>> 3` | `>> 2` | `lsr #3` | `srdi 3` |
| TOC needed? | No | No | No | Yes -- `r2` |
| Caller-save scratch pool | `rax,rcx,rsi,rdi,r8,r9,r12-r14` | `eax,ecx,edi` | `x11-x17` | `r16-r22` |
| `THREAD_WAIT` (STA) | empty stub | empty stub | empty stub | empty stub |

## JIT inline templates

Each opcode has at least one inline template `inline %NNh` in `core60.asm`.
The JIT copies the template body into the output buffer and patches
relocation slots. Some opcodes have multiple template variants selected by
an argument-driven prefix (e.g. `%075h`, `%175h`, `%275h` ...). The prefix is
chosen by `JITCompilerScope` based on the operand kind (immediate width,
dp/sp/fp form, reference vs. constant, etc.). See
[`../jit-passes.md`](../jit-passes.md) for the full selector catalogue.

## Index

### Frame / control flow
| Hex | Mnemonic | Enum |
|---|---|---|
| `0x00` | nop | Nop |
| `0x01` | breakpoint | Breakpoint |
| `0x02` | snop | SNop |
| `0x04` | quit | Quit |
| `0x32` | xnop | XNop |
| `0x34` | xquit | XQuit |
| `0x91` | close n | CloseN |
| `0xCA` | extclose n | ExtCloseN |
| `0xF0` | open i,n | OpenIN |
| `0xF2` | extopen i,n | ExtOpenIN |
| `0xDE` | xopen i,n | XOpenIN |
| `0x1001` | label | Label |

### Object / GC
See `0x07 len`, `0x08 class`, `0x15 mlen`, `0x2D parent`, `0x82 nlen`,
`0x8F creater`, `0xCE xcreater`, `0xE7 xnewnr`, `0xF4 newir`, `0xF5 newnr`,
`0xF7 createnr`, `0xF8 fillir`.

### Arithmetic (index/`rdx`)
`0x21 not`, `0x22 neg`, `0x1F lneg`, `0x75 shl`, `0x76 shr`, `0x8C subn`,
`0x8D addn`, `0x94 andn`, `0x9B orn`, `0x9C muln`.

### Arithmetic on `dp[i]`
`0xD0 fadddpn` ... `0xD3 fdivdpn`, `0xD4 udivdpn`, `0xD8 ianddpn` ... `0xDD ishrdpn`,
`0xE1 iadddpn` ... `0xE4 idivdpn`, `0xE8 nadddpn`.

### Float scalar (dp form)
`0x78 fabsdp`, `0x79 fsqrtdp`, `0x7A fexpdp`, `0x7B flndp`, `0x7C fsindp`,
`0x7D fcosdp`, `0x7E farctandp`, `0x7F fpidp`, `0x98 nconvfdp`, `0x99 ftruncdp`,
`0x9F frounddp`.

### Compare / branch
`0x0D xcmp`, `0x1C xlcmp`, `0x97 cmpn`, `0xC0 cmpr`, `0xC1 fcmpn`, `0xC2 icmpn`,
`0xC3 tstflag`, `0xC4 tstn`, `0xC5 tstm`, `0xC6 xcmpsi`, `0xC8 cmpfi`, `0xC9 cmpsi`,
`0xB2 jump`, `0xB3 jeq`, `0xB4 jne`, `0xB7 jlt`, `0xB8 jge`, `0xB9 jgr`, `0xBA jle`,
`0x17 tststck`.

### Call / dispatch
`0x03 redirect`, `0x27 xjump`, `0x2F xcall`, `0xB0 callr`, `0xB1 callvi`,
`0xB5 jumpvi`, `0xB6 xredirectm`, `0xEC vjumpmr`, `0xED jumpmr`, `0xFA xdispatchmr`,
`0xFB dispatchmr`, `0xFC vcallmr`, `0xFD callmr`, `0xFE callextr`.

### Load / store
`0x06 load`, `0x0E bload`, `0x0F wload`, `0x14 loads`, `0x1A lload`, `0x1D xload`,
`0x1E xlload`, `0x09 save`, `0x24 lsave`, `0x25 fsave`, `0x30 xfsave`,
`0x83 xassigni`, `0xA0 savedp`, `0xA1 storefi`, `0xA2 savesi`, `0xA3 storesi`,
`0x8A loaddp`, `0xAA lsavedp`, `0xAB lsavesi`, `0xAC lloaddp`, `0xCB lloadsi`,
`0xCC loadsi`, `0xCD xloadargfi`.

### Stack
`0x16 dalloc`, `0x35 dfree`, `0x92 alloci`, `0x93 freei`, `0xAF setsp`,
`0xA7 xrefreshsi`, `0xA4 xflushsi`, `0xAE xstorei`, `0xF1 xstoresir`,
`0xF9 xstorefir`, `0xF3 movsifi`, `0xF6 xmovsisi`, `0x86 xswapsi`, `0x87 swapsi`,
`0xA8 peekfi`, `0xA9 peeksi`, `0x84 peekr`, `0x85 storer`.

### Exceptions / hooks
`0x0A throw`, `0x0B unhook`, `0x10 exclude`, `0x11 include`, `0xD6 xlabeldpr`,
`0xE6 xhookdpr`.

### Selection (cmov)
`0xD7 selgrrr`, `0xDF selultrr`, `0xEE seleqrr`, `0xEF selltrr`, `0x2A xpeekeq`.

### Locking / threading
`0x2B trylock`, `0x2C freelock`, `0xBB peektls`, `0xBC storetls`, `0xCF system`.

### Misc / meta
`0x05 movenv`, `0x13 movfrm`, `0x18 dtrans`, `0x19 xassign`, `0x12 assign`,
`0x20 coalesce`, `0x23 bread`, `0x26 wread`, `0x28 bcopy`, `0x29 wcopy`,
`0x2E xget`, `0x31 altmode`, `0x88 movm`, `0x89 movn`, `0x8B xcmpdp`,
`0x90 copy`, `0x95 readn`, `0x96 writen`, `0x9A dcopy`, `0x9D xadddp`,
`0xBD xladddp`, `0xE0 copydpn`, `0xE5 nsavedpn`, `0xE9 dcopydpn`,
`0xEA xwriteon`, `0xEB xcopyon`, `0xD5 xsavedispn`, `0x77 xsaven`,
`0xAD xfillr`, `0x80 setr`, `0x81 setdp`, `0x8E setfp`, `0x9E xsetfp`.
