# 0xFE -- call extern

- **Category:** TripleOp
- **Enum:** `ByteCode::CallExtR`
- **Operand(s):** `call extern:r, n` -- `n` argument count

## Semantics
Invoke an external (C ABI) routine. The first `n` arguments are loaded from the ELENA shadow stack / sp[*] registers into the platform's native argument registers (or pushed on x32 cdecl), the call is issued, and the return value is moved into `index`.

## JIT (amd64)
**Selection rule:** `loadCallOp` (`jitcompiler.cpp:1488-1545`). Bit 31 of `arg2` acts as an "extension flag" -- when set, `prefix = 6` (alt-extended call). The lower bits of `arg2`  in  {0,1,2,3,4} pick inline slots {1,2,3,4,5}; the default slot is 0 (full register-load body). `arg1` is the external symbol ref (resolved via `mskExternalRef`). The arg count `n` drives both the per-arch argument-loading body (see [`../runtime.md`](../runtime.md) -- External call ABI per arch) and the variant slot.

**Template:** `asm/amd64/core60.asm` `inline %0FEh`

amd64 has both Win64 and SysV variants, selected at assembly time via `#if`. Win64 uses `rcx/rdx/r8/r9` with 32-byte shadow space; SysV uses `rdi/rsi/rdx/rcx/r8/r9`. Args beyond register count are loaded from `rsp + offset` (the caller already placed them there).

```asm
; generic %0FEh: up to 6 args (SysV) / 4 args (Win64)
#if _WIN
  mov  r9,  [rsp+24]
  mov  r8,  [rsp+16]
  mov  rdx, r11        ; sp[1]
  mov  rcx, r10        ; sp[0]
#elif (_LNX || _FREEBSD)
  mov  r9,  [rsp+40]
  mov  r8,  [rsp+32]
  mov  rcx, [rsp+24]
  mov  rdx, [rsp+16]
  mov  rsi, r11
  mov  rdi, r10
#endif

call extern __relptr32_1
mov  rdx, rax            ; index := return value
```

### Variants
| Prefix | Args (n) | Body |
|---|---|---|
| `%0FEh` | full (4 Win64 / 6 SysV) | as above |
| `%1FEh` | 0 args | just `call extern; mov rdx, rax` |
| `%2FEh` | 1 arg | `rcx`/`rdi` <- `r10` |
| `%3FEh` | 2 args | `rcx,rdx`/`rdi,rsi` |
| `%4FEh` | 3 args | + 3rd arg from `[rsp+16]` |
| `%5FEh` | 4 args | + 4th arg from `[rsp+24]` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0FEh`

x32 uses cdecl -- args are **pushed onto the stack right-to-left** by the caller before the call; this opcode itself just issues the call. The `n` argument count is reflected only in caller-side push generation, not in the inline body.

```asm
; %0FEh -- preserves esi (sp[0] shadow) across call
mov  [esp], esi
call extern __ptr32_1
mov  edx, eax
```

### Variants
| Prefix | Body |
|---|---|
| `%0FEh` | preserve `esi`, set `edx <- eax` |
| `%1FEh` | just `call; mov edx, eax` (no esi save) |
| `%6FEh` | preserve `esi`, no return-to-edx |
| `%7FEh` | bare `call extern` |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0FEh`

AAPCS -- args in `x0..x7`. `x0`/`x1` already hold sp[0]/sp[1] (ELENA convention matches AAPCS). Extra args loaded from `[sp+16]`, `[sp+24]`.

```asm
add     x15, sp, #16
ldr     x2, [x15]
ldr     x3, [x15, #8]

#if _MAC
  adrp    x16, __ptr32page_1
  add     x16, x16, __ptr32pageoff_1
#elif (_LNX || _FREEBSD)
  movz    x16, __ptr32lo_1
  movk    x16, __ptr32hi_1, lsl #16
#endif
  ldr     x17, [x16]
  blr     x17
  mov     x9, x0           ; index := return value
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0FEh`

ELFv2 -- args in `r3..r10`. `r3`/`r4` already hold sp[0]/sp[1]. Extra args from `[r1+16]`, `[r1+24]`. Call goes through the import address table; TOC (`r2`) must be reloaded after return.

```asm
; %0FEh
ld      r5, 16(r1)
ld      r6, 24(r1)

ld      r12, toc_import(r2)
addis   r12, r12, __disp32hi_1
ld      r12, __disp32lo_1(r12)

mtctr   r12
bctrl

mr      r14, r3              ; index := return value

; reload TOC pointer (callee may have clobbered it)
lis   r2, rdata32_hi : %CORE_TOC
addi  r2, r2, rdata32_lo : %CORE_TOC
```

## Notes
- `n` is the argument count; argument passing follows the per-arch C ABI -- see [`../runtime.md`](../runtime.md) for the register/stack mapping.
- Bit 31 of `arg2` switches to extended-call mode (`prefix = 6`); lower bits select inline slot 0-5 based on the argument count.
- `arg1` is resolved as `mskExternalRef` -- the import-table address is patched in by the linker.
- On x32 cdecl the caller emits the right-to-left arg pushes; this opcode just issues the `call` and copies `eax -> edx`.
- ppc64le reloads the TOC (`r2`) after return because the external callee may have clobbered it.
- Return value is moved into the `index` register (`rdx`/`edx`/`x9`/`r14`) per arch.
