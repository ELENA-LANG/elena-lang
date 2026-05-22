# 0x8B -- xcmp dp

- **Category:** DoubleOp
- **Enum:** `ByteCode::XCmpDP`
- **Operand(s):** `xcmp dp:i`
- **Reads:** `index`, `fp`, `dp[i]`
- **Writes:** condition flags (COMP.EQ)
- **Side effects:** Sets architecture condition codes.

## Semantics
`COMP.EQ := (index == dp[i])` -- 32-bit comparison of `index` against the frame slot. Mirrors `xcmp` (0x0D) which compares against `[acc]`. For 64-bit see `xlcmp` (0x1C).

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %8Bh`

**Selection rule:** `retrieveCode` selects the offset-encoding variant; the argument is rewritten as `getFPOffset(arg1, dataOffset)` so the emitted displacement is the actual frame-relative byte offset.

```asm
  mov  ecx, dword ptr [rbp + __arg32_1]
  cmp  edx, ecx
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %8Bh`

```asm
  mov  ecx, [ebp + __arg32_1]
  cmp  edx, ecx
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %8Bh`

```asm
  add     x11, x29, __arg12_1
  ldrsw   x14, [x11]
  cmp     x9, x14
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %8Bh`

```asm
  addi    r16, r31, __arg16_1
  lwz     r18, 0(r16)
  cmp     r14, r18
```

## Notes
- 32-bit compare; mirrors `xcmp` (0x0D) which compares against `[acc]`. For 64-bit, use `xlcmp` (0x1C).
- Argument is the abstract dp index; JIT rewrites it via `getFPOffset` before emit.
- Only condition flags are written -- `index`, `acc`, `sp[0..1]` are all preserved.
- Subsequent `jeq`/`jne`/`jlt`/`jge`-style opcodes consume the resulting flags.
