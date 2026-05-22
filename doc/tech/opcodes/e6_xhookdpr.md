# 0xE6 -- xhook dp

- **Category:** TripleOp
- **Enum:** `ByteCode::XHookDPR`
- **Operand(s):** `xhook dp:disp, r` (R-cmd)

## Semantics
Install a try/catch hook record at `dp[disp]`:
- `es_prev_struct` = previous `et_current` (chain link)
- `es_catch_frame` = current frame pointer
- `es_catch_level` = current stack pointer
- `es_catch_addr` = handler address `r`
Then update `et_current` to point to this new record. Single-thread version uses `CORE_SINGLE_CONTENT.et_current` directly; MT version dereferences per-thread storage via TLS.

## Compiler behaviour
`compileHookDPR` (`jitcompiler.cpp:2861-2868`) bifurcates on `arg2 & mskLabelRef`:
- If the label-ref flag is set, it calls `loadDPLabelOp` and emits a label-relative PC offset for the handler.
- Otherwise it calls `loadDPROp`, which patches an absolute handler address (via `mskCodeRef`).

`arg1` becomes the `dp:disp` of the exception record on the current frame; the handler PC goes into `es_catch_addr`. In MT mode the per-thread `et_current` slot is touched through the TLS-loaded thread frame instead of `CORE_SINGLE_CONTENT.et_current`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %0E6h`
```asm
lea  rdi, [rbp + __arg32_1]
mov  rcx, __ptr64_2
mov  rax, [data : %CORE_SINGLE_CONTENT + et_current]

mov  [rdi + es_prev_struct], rax
mov  [rdi + es_catch_frame], rbp
mov  [rdi + es_catch_level], rsp
mov  [rdi + es_catch_addr], rcx

mov  [data : %CORE_SINGLE_CONTENT + et_current], rdi
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0E6h`
```asm
lea  edi, [ebp + __arg32_1]
mov  eax, [data : %CORE_SINGLE_CONTENT + et_current]

mov  [edi + es_prev_struct], eax
mov  [edi + es_catch_frame], ebp
mov  [edi + es_catch_level], esp
mov  [edi + es_catch_addr], __ptr32_2

mov  [data : %CORE_SINGLE_CONTENT + et_current], edi
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0E6h`
```asm
add     x13, x29, __arg12_1

; load &CORE_SINGLE_CONTENT (Mach-O adrp / ELF movz+movk)
movz    x14, data_ptr32lo : %CORE_SINGLE_CONTENT
movk    x14, data_ptr32hi : %CORE_SINGLE_CONTENT, lsl #16
mov     x18, x13

; load handler address r
movz    x16, __ptr32lo_2
movk    x16, __ptr32hi_2, lsl #16
add     x14, x14, # et_current

mov     x17, sp
ldr     x15, [x14]

str     x15, [x13]            ; es_prev_struct
str     x16, [x13, #8]!       ; es_catch_addr
str     x17, [x13, #8]!       ; es_catch_level
str     x29, [x13, #8]!       ; es_catch_frame
str     x18, [x14]            ; et_current = &record
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0E6h`
```asm
addi    r19, r31, __arg16_1

ld      r14, toc_data(r2)
addis   r14, r14, data_disp32hi : %CORE_SINGLE_CONTENT
addi    r14, r14, data_disp32lo : %CORE_SINGLE_CONTENT

ld      r15, et_current(r14)

ld       r12, toc_code(r2)
addis    r12, r12, __disp32hi_2
addi     r12, r12, __disp32lo_2

std     r15, es_prev_struct(r19)
std     r12, es_catch_addr(r19)
std     r1,  es_catch_level(r19)
std     r31, es_catch_frame(r19)

std     r19, et_current(r14)
```

## MT variant (corex60.asm / GCXT)

Replaces the global `et_current` slot with a per-thread frame loaded via OS TLS.

### amd64
**Template:** `asm/amd64/corex60.asm` `inline %0E6h`
```asm
; GCXT: get current thread frame via gs:[58h]
mov  rcx, gs:[58h]
lea  rdi, [rbp + __arg32_1]
mov  rax, [rcx]

mov  rcx, [rax + et_current]
mov  [rdi + es_catch_frame], rbp
mov  [rdi + es_prev_struct], rcx
mov  [rdi + es_catch_level], rsp
mov  rcx, __ptr64_2
mov  [rdi + es_catch_addr], rcx

mov  [rax + et_current], rdi
```

### x32
**Template:** `asm/x32/corex60.asm` `inline %0E6h`
```asm
; GCXT: get current thread frame
#if _WIN
  mov  eax, fs:[2Ch]
#elif _LNX
  mov  eax, gs:[0]
#endif

lea  edi, [ebp + __arg32_1]
#if _WIN
  mov  eax, [eax]
#elif _LNX
  lea  eax, [eax-tt_size]
#endif

mov  ecx, [eax + et_current]
mov  [edi + es_catch_frame], ebp
mov  [edi + es_prev_struct], ecx
mov  [edi + es_catch_level], esp
mov  [edi + es_catch_addr], __ptr32_2

mov  [eax + et_current], edi
```

## Notes
- Installs a try/catch record at `fp + dp:disp` on the current frame.
- Pairs with `unhook` (0x0B) for normal exit and `throw` (0x0A) for unwinding.
- MT build threads the chain through per-thread TLS (`gs:[58h]` on amd64 Linux, `fs:[2Ch]` on x32 Windows, `gs:[0]` on x32 Linux).
- `arg2 & mskLabelRef` selects between label-relative and absolute handler addressing -- the assembler picks the right variant when emitting `xhook`.
