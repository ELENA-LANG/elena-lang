# 0xAC -- lload dp

- **Category:** DoubleOp
- **Enum:** `ByteCode::LLoadDP`
- **Operand(s):** `lload dp:disp`
- **Reads:** `dp[disp]` (full word)
- **Writes:** `index` (full pointer-wide; `eax:edx` pair on x32)
- **Side effects:** None.

## Semantics
`long:index := dp[disp]` -- load a full-width value from the data-frame slot at `disp` into the index register. Companion of `lsave dp:disp` (0xAA).

## JIT (amd64)

**Selection rule:** dispatched through `loadFrameIndexOp` (`jitcompiler.cpp:980`) -- `retrieveCode(arg1)` picks the inline template and the operand slot is rewritten as `getFPOffset(arg1 << indexPower, frameOffset)`. The shared template uses `lea` + indirect `mov` rather than a single displacement `mov` so it can be reused by the x32 split-word load.

**Template:** `asm/amd64/core60.asm` `inline %0ACh`

```asm
  lea  rdi, [rbp + __arg32_1]
  mov  rdx, [rdi]
```

The `lea`+`mov` pair (rather than a direct `mov rdx, [rbp+disp]`) is the shared template; `rdi` ends up holding the slot address.

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0ACh`

```asm
  lea  edi, [ebp + __arg32_1]
  mov  eax, dword ptr [edi]
  mov  edx, dword ptr [edi+4]
```

Long lives in `eax:edx`: low word into `eax`, high word into `edx`.

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0ACh`

```asm
  add     x11, x29, __arg12_1
  ldr     x9,  [x11]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0ACh`

```asm
  addi    r16, r31, __arg16_1
  ld      r14, 0(r16)
```

## Notes

- Full-width load of `dp[disp]` into `index`. Counterpart to `lsave dp:disp` (0xAA).
- On x32 the destination is the `eax:edx` pair: low half at `[ebp+disp]`, high half at `[ebp+disp+4]`.
- 32-bit-only sibling is `xload dp:disp` (0x99) family.
- Preserves `acc` and the stack cache.
