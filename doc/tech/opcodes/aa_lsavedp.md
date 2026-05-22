# 0xAA -- lsave dp

- **Category:** DoubleOp
- **Enum:** `ByteCode::LSaveDP`
- **Operand(s):** `lsave dp:disp`
- **Reads:** `index` (full pointer-wide value; `eax:edx` pair on x32)
- **Writes:** `dp[disp]` (full word)
- **Side effects:** None.

## Semantics
`dp[disp] := long:index` -- store the full-width `index` (pointer-sized) into the data-frame slot at `disp`. Long counterpart of `save dp:disp` (0xA0). Use this when the value may exceed 32 bits or carries a tagged pointer in its high half (`mov env`, `mov frm`, addresses).

## JIT (amd64)

**Selection rule:** dispatched through `loadFrameIndexOp` (`jitcompiler.cpp:980`) -- `retrieveCode(arg1)` picks the inline template and the operand slot is rewritten as `getFPOffset(arg1 << indexPower, frameOffset)`. Unlike `save dp:disp` (0xA0), the emitted store is full-width (pointer-sized), so on x32 the JIT splits `eax:edx` into two consecutive frame words.

**Template:** `asm/amd64/core60.asm` `inline %0AAh`

```asm
  mov  [rbp + __arg32_1], rdx
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0AAh`

```asm
  lea  edi, [ebp + __arg32_1]
  mov  [edi], eax
  mov  [edi + 4], edx
```

On x32 a "long" is the `eax:edx` pair -- `eax` is the low word, `edx` the high word -- stored into two consecutive frame slots.

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0AAh`

```asm
  add     x11, x29, __arg12_1
  str     x9, [x11]
```

`x9 = index` is already 64 bits, so a single `str` suffices.

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0AAh`

```asm
  std     r14, __arg16_1(r31)  ; save frame pointer
```

## Notes

- Full-width spill of `index` to `dp[disp]`. On 64-bit hosts this is the canonical way to save a tagged pointer/environment slot.
- On x32 the "long" is the `eax:edx` pair: `eax` at `[ebp+disp]`, `edx` at `[ebp+disp+4]` -- consecutive frame slots.
- Inverse: `lload dp:disp` (0xAC).
- Use `save dp:disp` (0xA0) for the truncating 32-bit form.
