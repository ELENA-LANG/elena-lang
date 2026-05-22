# 0xA0 -- save dp

- **Category:** DoubleOp
- **Enum:** `ByteCode::SaveDP`
- **Operand(s):** `save dp:disp`
- **Reads:** `index`, `fp`
- **Writes:** `dp[disp]` (32-bit slot)
- **Side effects:** Truncating 32-bit store into the current data frame.

## Semantics
`dp[disp] := index` -- store the low 32 bits of `index` into the data-frame slot at `disp` (relative to `fp`). The companion full-word store is `lsave dp:disp` (0xAA).

## JIT (amd64)

**Selection rule:** `loadFrameIndexOp` (`jitcompiler.cpp:980`) calls `retrieveCode(arg1)` to pick the inline template and then patches the operand slot with `getFPOffset(arg1 << indexPower, frameOffset)`. The bytecode operand is a *frame-slot index*, but the emitted instruction takes a *byte displacement from `rbp`* -- the JIT rewrites the slot index into a frame-relative byte offset and chooses the short/near displacement encoding from the slot magnitude.

**Template:** `asm/amd64/core60.asm` `inline %0A0h`

```asm
  mov  dword ptr[rbp + __arg32_1], edx
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0A0h`

```asm
  mov  [ebp + __arg32_1], edx
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0A0h`

```asm
  add     x11, x29, __arg12_1
  str     w9, [x11]
```

32-bit store (`str w9`) -- `index` is `x9`, frame pointer is `x29`.

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0A0h`

```asm
  stw     r14, __arg16_1(r31)  ; save frame pointer
```

`stw` is the 32-bit store; `r14 = index`, `r31 = fp`.

## Notes

- Truncating 32-bit store into the data frame at `dp[disp]`.
- Preserves `acc`, `sp[0..1]` cache, and the high half of `index` on 64-bit hosts.
- Use the 64-bit companion `lsave dp:disp` (0xAA) for full-width spills such as `mov env`, `mov frm`, or any tagged pointer.
- `disp` is a slot index in the bytecode; the JIT rewrites it to a byte offset via `getFPOffset(disp << indexPower, frameOffset)`.
