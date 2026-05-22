# 0xCA -- extclose n

- **Category:** DoubleOp
- **Enum:** `ByteCode::ExtCloseN`
- **Operand(s):** `extclose n` (unmanaged stack to release)
- **Reads:** `fp`, saved frame slots
- **Writes:** all callee-saved GPRs (restored), `sp`, `fp`, `tt_stack_frame`

## Semantics
External-entry epilogue, paired with `extopen i, n`. Releases the n-byte unmanaged stack, restores the previous frame, refreshes `tt_stack_frame` from the saved-frame slot, and pops all callee-saved registers.

## Compiler behaviour
Dispatched through `compileClose` (`jitcompiler.cpp:2919`): if `arg1 > 0` it is biased by `dataHeader` before the inline is emitted, and `stackOffset` is reset to `unframedOffset` so subsequent slot references in this scope use the post-close stack model. The `n` operand is the same number that was passed to the matching `ExtOpenIN` (0xF2); in the MT runtime the saved-`tt_stack_frame` slot is reloaded from per-thread TLS (`gs:[58h]` on amd64 SysV, `fs:[2Ch]` on x32 Windows, `gs:[0]` on x32 Linux), while in STA the link is direct via `CORE_SINGLE_CONTENT`.

## JIT (amd64) -- STA / `core60.asm`
**Template:** `add rbp, n; mov rsp, rbp; pop rbp` peels the unmanaged region; the next block restores the thread's `tt_stack_frame` and pops all preserved GPRs (Windows pops `rdi`/`rsi` too).

```asm
add  rbp, __n_1
mov  rsp, rbp
pop  rbp

add  rsp, 16
pop  rbx
mov  [data : %CORE_SINGLE_CONTENT + tt_stack_frame], rbx

pop  rbp
pop  r15
pop  r14
pop  r13
pop  r12
pop  rbx

; _WIN
pop  rdi
pop  rsi
add  rsp, 8

; _LNX || _FREEBSD
add  rsp, 40
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0CAh` | generic `extclose n` | as above |
| `%1CAh` | `extclose 0` | omits `add rbp, n` |

## JIT (x32) -- STA / `core60.asm`
**Template:** mirror layout with 8-byte spill region; pops `ebx/ecx/edi/esi` (and only the unsigned `pop ebp` for the saved frame link).

```asm
add  ebp, __n_1
mov  esp, ebp
pop  ebp

add  esp, 8
pop  ebx
mov  [data : %CORE_SINGLE_CONTENT + tt_stack_frame], ebx

pop  ebp
pop  ebx
pop  ecx
pop  edi
pop  esi
```

## JIT (aarch64)
**Template:** unwind the unmanaged region, then `ldp x29, x30, [sp], #16` for each saved pair; refresh `tt_stack_frame` from the prologue slot.

```asm
add     x29, x29, __n12_1
mov     sp, x29
ldp     x29, x30, [sp], #16

; load CORE_SINGLE_CONTENT base
movz    x14, data_ptr32lo : %CORE_SINGLE_CONTENT
movk    x14, data_ptr32hi : %CORE_SINGLE_CONTENT, lsl #16
ldr     x12, [x29, #8]
str     x12, [x14, # tt_stack_frame]

add     x29, x29, #16
mov     sp, x29

ldp     x29, x30, [sp], #16
ldp     x27, x28, [sp], #16
ldp     x25, x26, [sp], #16
ldp     x23, x24, [sp], #16
ldp     x21, x22, [sp], #16
ldp     x19, x20, [sp], #16
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0CAh` | generic | as above |
| `%1CAh` | `extclose 0` | omits `add x29, x29, n` |

## JIT (ppc64le)
**Template:** restore the unmanaged stack via `r31`, pop preserved GPRs and the LR from the standard ELF v2 save area.

```asm
addi  r31, r31, __n16_1     ; skip unframed stack
mr    r1, r31               ; restore stack pointer
addi  r1, r1, 32

ld   r31, 168(r1)
ld   r30, 160(r1)
ld   r29, 152(r1)
ld   r28, 144(r1)
ld   r27, 136(r1)
ld   r0,  128(r1)

mtlr r0

addi  r1, r1, 176
```

## MT variant (corex60.asm / GCXT)

### amd64
**Template:** identical layout but `tt_stack_frame` is read from the per-thread `gs:[58h]` (TEB / thread-table pointer) instead of the global `CORE_SINGLE_CONTENT`.

```asm
add  rbp, __n_1
mov  rsp, rbp
pop  rbp

add  rsp, 16
pop  rbx

mov  rcx, gs:[58h]
mov  rdi, [rcx]
mov  [rdi + tt_stack_frame], rbx

pop  rbp
pop  r15
pop  r14
pop  r13
pop  r12
pop  rbx
pop  rdi
pop  rsi
add  rsp, 8
```

### x32
**Template:** Windows uses `fs:[2Ch]`, Linux uses `gs:[0]`; otherwise the same shape as STA.

```asm
add  ebp, __n_1
mov  esp, ebp
pop  ebp

add  esp, 8
pop  ebx

; _WIN
mov  ecx, fs:[2Ch]
mov  edi, [ecx]
; _LNX
mov  ecx, gs:[0]
lea  edi, [ecx - tt_size]

mov  [edi + tt_stack_frame], ebx

pop  ebp
pop  ebx
pop  ecx
pop  edi
pop  esi
```

## Notes
- Pairs with `extopen i, n` (0xF2); the `n` operand matches the unmanaged-stack size passed at open.
- In MT (corex60.asm) restores the per-thread `tt_stack_frame` from a saved slot via TLS; in STA the link is direct through `CORE_SINGLE_CONTENT`.
- Pops callee-saved GPRs: amd64 (rbx, r12-r15, rbp; Windows also rsi/rdi); x32 (esi, edi, ecx, ebx); aarch64 (x19-x29); ppc64le (r27-r31).
- Variant `%1CAh` (`extclose 0`) omits the leading `add rbp, n` since there is no unmanaged region to release.
- Must restore `tt_stack_frame` so the next call into ELENA sees the previous managed frame chain -- leaving a stale value risks GC root-set corruption.
