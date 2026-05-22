# 0xCF -- system

- **Category:** DoubleOp
- **Enum:** `ByteCode::System`
- **Operand(s):** `system n` (subcommand selector)
- **Reads:** depends on subcommand
- **Writes:** depends on subcommand

## Semantics
Invokes a VM service. The first byte of the immediate selects the subcommand:

## Compiler behaviour
Dispatched through `loadSysOp` (`jitcompiler.cpp:507-523`) -- NOT `retrieveICode`. Direct switch on the literal `n` value: cases 1-5 map straight to inline slots 1-5; any other value (including 0, 6, 7) silently routes to slot 0. The `%6CFh`/`%7CFh` inlines for "enter/leave GC critical" exist in `corex60.asm` but are reached through other paths (typically the runtime call chain wired up by the open/close machinery), not through this direct dispatch. Subcmd 4 (`startup` / `%PREPARE`) and subcmd 3 (thread register) interact with runtime globals -- they must run exactly once per process / thread, in the right order, or the GC tables will be in an inconsistent state.

The discriminator is `n`:

| n | Mnemonic | Effect |
|---|---|---|
| 0 | `system` | no-op (placeholder) |
| 1 | minor collect | forced minor GC |
| 2 | major collect | forced full GC |
| 3 | thread startup | record TLS slot, save thread `tt_stack_root` |
| 4 | startup / prepare | program entry: `finit`, capture `tt_stack_root`, call `PREPARE` |
| 5 | inject stack | reserve `index`-many slots on the unmanaged stack (zero-filled) |
| 6 | enter GC critical | spin-acquire `gc_lock` |
| 7 | leave GC critical | release `gc_lock` |

## JIT (amd64)
### Variants (`core60.asm` -- STA)
| Prefix | Subcmd | Body |
|---|---|---|
| `%0CFh` | 0 | empty |
| `%1CFh` | 1 minor GC | `xor ecx, ecx; call %GC_COLLECT` |
| `%2CFh` | 2 major GC | `mov ecx, 1; call %GC_COLLECT` |
| `%4CFh` | 4 startup | `finit`; capture `rsp` -> `tt_stack_root`; OS-specific shim then `call %PREPARE` |
| `%5CFh` | 5 stack alloc | pop saved sp[0], align `rdx`, `sub rsp, rdx*8`, zero-fill via `rep stos`, restore sp[0] |

```asm
; %4CFh -- startup (Linux)
finit
mov  rax, rsp
mov  [data : %CORE_SINGLE_CONTENT + tt_stack_root], rsp
call %PREPARE
xor  rbp, rbp
push rbp                ; extra push to simulate function entry
```

```asm
; %5CFh -- stack alloc
pop  rsi
add  rdx, 1
and  rdx, 0FFFFFFFEh
lea  rax, [rdx*8]
sub  rsp, rax
mov  rcx, rdx
xor  rax, rax
mov  rdi, rsp
rep  stos
push rsi
```

## MT variant (corex60.asm / GCXT) -- amd64
All GC subcommands first spin-acquire the global `gc_lock` via `lock cmpxchg`; subcmd 3 stores the thread frame into `tt_slots[index]`; subcmds 6 / 7 manipulate the lock directly.

| Prefix | Subcmd | Body |
|---|---|---|
| `%1CFh` | minor GC | spin on `gc_lock`, then `call %GC_COLLECT` (rcx=0) |
| `%2CFh` | major GC | spin on `gc_lock`, then `call %GC_COLLECT` (rdx=1) |
| `%3CFh` | thread startup | read `gs:[58h]`, store thread block into `tt_slots[rdx*16]`, save `rsp` to `tt_stack_root` |
| `%4CFh` | startup | `finit; mov rax, rsp; call %PREPARE` |
| `%6CFh` | enter GC critical | `lock cmpxchg` spin on `gc_lock` setting to 1 |
| `%7CFh` | leave GC critical | `lock xadd [gc_lock], 0FFFFFFFFh` to release |

```asm
; %6CFh -- enter GC critical
mov  rdi, data : %CORE_GC_TABLE + gc_lock
mov  ecx, 1
labWait:
  xor  eax, eax
  lock cmpxchg dword ptr [rdi], ecx
  jnz  short labWait
```

```asm
; %7CFh -- leave GC critical
mov  rdi, data : %CORE_GC_TABLE + gc_lock
mov  ecx, 0FFFFFFFFh
lock xadd [rdi], ecx
```

## JIT (x32)
### Variants (`core60.asm` -- STA)
| Prefix | Subcmd | Body |
|---|---|---|
| `%0CFh` | 0 | empty |
| `%1CFh` | 1 minor GC | `xor ecx,ecx; xor edx,edx; call %GC_COLLECT` |
| `%2CFh` | 2 major GC | `xor ecx,ecx; mov edx,1; call %GC_COLLECT` |
| `%4CFh` | 4 startup | `finit; mov [tt_stack_root], esp; mov eax, esp; call %PREPARE` |
| `%5CFh` | 5 stack alloc | move-and-zero like amd64, scaled `*4` |

## MT variant (corex60.asm / GCXT) -- x32
Windows uses `fs:[2Ch]` (NT_TIB.ArbitraryUserPointer indirection), Linux uses `gs:[0]` (with `-tt_size` offset) to reach the thread block.

| Prefix | Subcmd | Body |
|---|---|---|
| `%1CFh` | minor GC | spin on `gc_lock`, `call %GC_COLLECT` |
| `%2CFh` | major GC | spin on `gc_lock`, `call %GC_COLLECT` (edx=1) |
| `%3CFh` | thread startup | resolve TLS, store thread block in `tt_slots[edx*8]`, save `esp` |
| `%4CFh` | startup | `finit; mov eax, esp; call %PREPARE` |
| `%6CFh` | enter GC critical | spin on `gc_lock` |
| `%7CFh` | leave GC critical | `lock xadd [gc_lock], 0FFFFFFFFh` |

## JIT (aarch64) -- STA only
| Prefix | Subcmd | Body |
|---|---|---|
| `%0CFh` | 0 | empty |
| `%4CFh` | startup | save `sp` to `tt_stack_root`, call `PREPARE` via materialised pointer |

```asm
; %4CFh
mov     x12, sp
movz    x14, data_ptr32lo : %CORE_SINGLE_CONTENT
movk    x14, data_ptr32hi : %CORE_SINGLE_CONTENT, lsl #16
add     x14, x14, # tt_stack_root
str     x12, [x14]
movz    x17, code_ptr32lo : %PREPARE
movk    x17, code_ptr32hi : %PREPARE, lsl #16
blr     x17
```

*Subcommands 1, 2, 3, 5, 6, 7 are not implemented in this backend.*

## JIT (ppc64le) -- STA only
| Prefix | Subcmd | Body |
|---|---|---|
| `%0CFh` | 0 | empty |
| `%4CFh` | startup | reload TOC, save `r1` to `tt_stack_root`, call `prepare` through TOC |

```asm
; %4CFh
lis     r2, rdata32_hi : %CORE_TOC
addi    r2, r2, rdata32_lo : %CORE_TOC

ld      r16, toc_data(r2)
addis   r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
addi    r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT
std     r1, tt_stack_root(r16)

ld      r12, toc_prepare(r2)
mtctr   r12
bctrl

lis     r2, rdata32_hi : %CORE_TOC
addi    r2, r2, rdata32_lo : %CORE_TOC
```

*Other subcommands not implemented in this backend.*

## Notes
- Discriminator is `n` (1=minor GC, 2=major, 3=thread register, 4=startup, 5=stack inject, 6=enter critical, 7=leave critical).
- `loadSysOp` only dispatches cases 1-5 directly; 0/6/7 fall through to slot 0 (no-op for 0; 6/7 inlines exist in `corex60.asm` but are reached via other paths in this branch).
- n=6/7 MUST bracket properly -- leaking the critical section deadlocks the GC.
- Most variants are runtime-only (no semantic in user code) -- they appear in the runtime's hand-written assembly stubs rather than compiler-emitted bytecode.
- Only amd64/x32 have full coverage (with separate STA and GCXT implementations). aarch64 and ppc64le currently only implement subcommand 4 (program prepare); the threading subcommands rely on the host runtime through other paths.
- The bytecode is overloaded: the same opcode covers GC, threading, TLS, stack injection and program startup.
