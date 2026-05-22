# 0xD6 -- xlabel dp

- **Category:** TripleOp
- **Enum:** `ByteCode::XLabelDPR`
- **Operand(s):** `xlabel dp:i, r` (R-cmd; code reference)
- **Reads:** code reference operand
- **Writes:** `dp[i]` (one pointer)

## Semantics
`dp[i] := address-of(r)` -- install a code label / handler pointer into a data-frame slot. Used for exception handlers, dispatch tables, etc.

## JIT (amd64)
**Selection rule:** R-command -- dispatched through `compileHookDPR` (`jitcompiler.cpp:2861-2868`), the same handler also serves `XHookDPR` at 0xE6. The variant key is the reference kind of `r`: `mskLabelRef` (intra-section label) routes to `loadDPLabelOp` (relative displacement encoding); any other kind routes to `loadDPROp` (absolute pointer materialisation). The frame slot index is converted via `getFPOffset(arg1)` before patching.

**Template:** materialise the absolute pointer, store as a qword.

```asm
lea  rdi, [rbp + __arg32_1]
mov  rcx, __ptr64_2
mov  [rdi], rcx
```

## JIT (x32)
**Template:** 32-bit relocated immediate stored at `[ebp + i]`.

```asm
lea  edi, [ebp + __arg32_1]
mov  eax, __ptr32_2
mov  [edi], eax
```

## JIT (aarch64)
**Template:** materialise the address (`movz/movk` or `adrp/add` on macOS), `str`.

```asm
; Linux/FreeBSD
add     x13, x29, __arg12_1
movz    x16, __ptr32lo_2
movk    x16, __ptr32hi_2, lsl #16
str     x16, [x13]
```

## JIT (ppc64le)
**Template:** TOC-relative load of the target code address, `std`.

```asm
addi    r19, r31, __arg16_1
ld      r12, toc_code(r2)
addis   r12, r12, __disp32hi_2
addi    r12, r12, __disp32lo_2
std     r12, 0(r19)
```

## Notes
- Pairs with `xhook dp:i, r` (0xE6) -- `xhook` registers the slot as the *current* exception handler (writes to the exception chain), while `xlabel` only fills the slot.
- Used in the prologue of `try` blocks before the chain is wired.
- Selection between intra-section labels (relative encoding) and other refs (absolute pointer) happens at JIT time via `compileHookDPR`.
- Writes a full pointer-width word to `dp[i]`; no flags touched.
