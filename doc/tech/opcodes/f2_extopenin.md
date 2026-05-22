# 0xF2 -- extopen

- **Category:** TripleOp
- **Enum:** `ByteCode::ExtOpenIN`
- **Operand(s):** `extopen i, n` -- `i` managed slots, `n` unmanaged bytes

## Semantics
External-entry prologue. Used when ELENA code is invoked from outside (callback, GUI shim, etc.). Saves all callee-saved registers per the platform ABI, sets up an inner ELENA frame (mirroring `open`), and links to the per-context `tt_stack_frame` so cross-frame walks can resume.

Single-thread version reads `CORE_SINGLE_CONTENT.tt_stack_frame`; MT version dereferences a per-thread frame through OS TLS.

## Compiler behaviour
`compileExtOpen` (`jitcompiler.cpp:2937-2943`) is the external-entry counterpart of `compileOpen`. It calls `calcFrameOffset(arg2, true)` -- the second arg `extended = true` flag -- which extends the unmanaged area to include the callee-saved register save block per the host ABI (Win64 home-space, SysV V64 register dump, AAPCS callee-saves, ELFv2 stack frame). The inner ELENA frame is then built the same way as `open`. Variant selection follows the same `(i, n)` shape table as `OpenIN`, but each slot also threads in the platform-specific callee-save prologue.

## JIT (amd64)
**Selection rule:** Same `(i, n)` shape table as `OpenIN` (0xF0), but each slot also emits the platform-specific callee-saved register save sequence. Variant indices are picked directly by `(i, n)` via `compileExtOpen`'s switch, not through `retrieveCode`.

**Template:** `asm/amd64/core60.asm` `inline %0F2h`
```asm
; generic
#if _WIN
  mov  [rsp+8], rcx          ; save Win64 home-space args
  mov  [rsp+16], rdx
  mov  [rsp+24], r8
  mov  [rsp+32], r9
  push 0
  push rsi
  push rdi
  push rbx
#elif (_LNX || _FREEBSD)
  push 0
  push rcx
  push rdx
  push rsi
  push rdi
  push rbx
#endif

push r12
push r13
push r14
push r15

push rbp
mov  rax, [data : %CORE_SINGLE_CONTENT + tt_stack_frame]
push rax

mov  rbp, rax
xor  eax, eax
push rbp
push rax
mov  rbp, rsp

; --- inner frame mirrors `open` ---
push rbp
xor  rax, rax
mov  rbp, rsp
sub  rsp, __n_2
push rbp
push rax
mov  rbp, rsp
mov  rcx, __n_1
sub  rsp, __arg32_1
mov  rdi, rsp
rep  stos
mov  r10, rax        ; zero sp[0] shadow
mov  r11, rax        ; zero sp[1] shadow
```

### Variants
| Prefix | (i, n) |
|---|---|
| `%0F2h` | generic |
| `%1F2h` | (0, n) |
| `%2F2h` | (1, n) |
| `%3F2h` | (2, n) |
| `%4F2h` | (3, n) |
| `%5F2h` | (4, n) |
| `%6F2h` | (i, 0) |
| `%7F2h` | (0, 0) |
| `%8F2h` | (1, 0) |
| `%9F2h` | (2, 0) |
| `%0AF2h` | (3, 0) |
| `%0BF2h` | (4, 0) |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0F2h`
```asm
; generic
push esi
push edi
push ecx
push ebx

push ebp
mov  eax, [data : %CORE_SINGLE_CONTENT + tt_stack_frame]
push eax

mov  ebp, eax
xor  eax, eax
push ebp
push eax
mov  ebp, esp

; --- inner frame ---
push ebp
xor  eax, eax
mov  ebp, esp
sub  esp, __n_2
push ebp
push eax
mov  ebp, esp
mov  ecx, __n_1
sub  esp, __arg32_1
mov  edi, esp
rep  stos
mov  esi, eax        ; zero sp[0] shadow
```

### Variants
Same shape as amd64: `%0F2h`, `%1F2h..%5F2h` for (i, n), `%6F2h..%0BF2h` for (i, 0).

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0F2h`
```asm
; generic
stp     x19, x20, [sp, #-16]!     ; AAPCS callee-saved
stp     x21, x22, [sp, #-16]!
stp     x23, x24, [sp, #-16]!
stp     x25, x26, [sp, #-16]!
stp     x27, x28, [sp, #-16]!
stp     x29, x30, [sp, #-16]!

; load CORE_SINGLE_CONTENT.tt_stack_frame
movz    x14, data_ptr32lo : %CORE_SINGLE_CONTENT
movk    x14, data_ptr32hi : %CORE_SINGLE_CONTENT, lsl #16
add     x14, x14, # tt_stack_frame

mov     x3, 0
ldr     x12, [x14]
stp     x3, x12, [sp, #-16]!

mov     x29, sp

; --- inner frame mirrors `open` ---
stp     x29, x30, [sp, #-16]!
mov     x29, sp

sub     sp, sp, __n12_2

mov     x11, #0
stp     x11, x29, [sp, #-16]!

mov     x29, sp

sub     sp, sp, __arg12_1
; ... zero-fill loop for managed slots
```

### Variants
| Prefix | (i, n) |
|---|---|
| `%0F2h` | generic |
| `%1F2h` | (0, n) |
| `%6F2h` | (i, 0) |
| `%7F2h` | (0, 0) |
| `%8F2h` | (1, 0) |
| `%9F2h` | (2, 0) |
| `%0AF2h` | (3, 0) |
| `%0BF2h` | (4, 0) |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0F2h`
```asm
; generic
; loading TOC pointer
lis     r2, rdata32_hi : %CORE_TOC
addi    r2, r2, rdata32_lo : %CORE_TOC

stdu  r1, -176(r1)                ; ELFv2 stack frame

mflr    r0
std   r31, 168(r1)                ; callee-saved
std   r30, 160(r1)
std   r29, 152(r1)
std   r28, 144(r1)
std   r27, 136(r1)
std   r0,  128(r1)                ; return address

; recover previous frame
ld    r16, toc_data(r2)
addis r16, r16, data_disp32hi : %CORE_SINGLE_CONTENT
addi  r16, r16, data_disp32lo : %CORE_SINGLE_CONTENT
ld    r10, tt_stack_frame(r16)

li    r16, 0
std   r10, -10h(r1)
std   r16, -08h(r1)

addi  r1, r1, -16
mr    r31, r1

; --- inner frame mirrors `open` ---
addi  r1, r1, -__n16_2

li    r16, 0
std   r31, -08h(r1)
std   r16, -10h(r1)
addi  r1, r1, -10h
mr    r31, r1

addi  r1, r1, -__arg16_1
; ... zero-fill loop
```

### Variants
| Prefix | (i, n) |
|---|---|
| `%0F2h` | generic |
| `%1F2h` | (0, n) |
| `%06F2h` | (i, 0) |
| `%7F2h` | (0, 0) |
| `%8F2h` | (1, 0) |
| `%9F2h` | (2, 0) |
| `%0AF2h` | (3, 0) |
| `%0BF2h` | (4, 0) |

## MT variant (corex60.asm / GCXT)

Replaces the `CORE_SINGLE_CONTENT.tt_stack_frame` lookup with a per-thread frame loaded through OS TLS.

### amd64
**Template:** `asm/amd64/corex60.asm` `inline %0F2h`
```asm
; generic
mov  [rsp+8], rcx          ; Win64 home save (always emitted in MT)
mov  [rsp+16], rdx
mov  [rsp+24], r8
mov  [rsp+32], r9

push 0
push rsi
push rdi
push rbx
push r12
push r13
push r14
push r15

push rbp

mov  rcx, gs:[58h]         ; TLS lookup
mov  rdi, [rcx]
mov  rax, [rdi + tt_stack_frame]
push rax

mov  rbp, rax
xor  eax, eax
push rbp
push rax
mov  rbp, rsp

; --- inner frame mirrors `open` ---
push rbp
xor  rax, rax
mov  rbp, rsp
sub  rsp, __n_2
push rbp
push rax
mov  rbp, rsp
mov  rcx, __n_1
sub  rsp, __arg32_1
mov  rdi, rsp
rep  stos
mov  r10, rax
mov  r11, rax
```

Same set of (i, n) variants as the single-thread version: `%0F2h`, `%1F2h..%5F2h`, `%6F2h..%0BF2h`.

### x32
**Template:** `asm/x32/corex60.asm` `inline %0F2h`
```asm
; generic
push esi
push edi
push ecx
push ebx

push ebp

#if _WIN
  mov  eax, fs:[2Ch]
  mov  edi, [eax]
#elif _LNX
  mov  eax, gs:[0]
  lea  edi, [eax-tt_size]
#endif

mov  eax, [edi + tt_stack_frame]
push eax

mov  ebp, eax
xor  eax, eax
push ebp
push eax
mov  ebp, esp

; --- inner frame ---
push ebp
xor  eax, eax
mov  ebp, esp
sub  esp, __n_2
push ebp
push eax
mov  ebp, esp
mov  ecx, __n_1
sub  esp, __arg32_1
mov  edi, esp
rep  stos
mov  esi, eax
```

## Notes
- External entry frame -- preserves Win64 / SysV / AAPCS / ELFv2 callee-saved registers on top of the standard ELENA prologue.
- Pairs with `extclose n` (0xCA).
- In MT builds, links the new frame into the per-thread `tt_stack_frame` chain via TLS instead of the global `CORE_SINGLE_CONTENT.tt_stack_frame`.
- Used as the JIT entry point for callbacks invoked from C/host code (GUI shims, signal handlers, OS callbacks).
- `calcFrameOffset(arg2, true)` extends the raw stack region for the callee-save block.
