# 0x9D -- xadd dp

- **Category:** DoubleOp
- **Enum:** `ByteCode::XAddDP`
- **Operand(s):** `xadd dp:disp`
- **Reads:** `index`, `fp`, `dp[disp]`
- **Writes:** `index`, condition flags

## Semantics
`index += [dp[disp]]` -- add a 32-bit slot from the data frame to the data accumulator. For 64-bit see `xladddp` (0xBD).

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %09Dh`

**Selection rule:** `retrieveCode` selects the offset-encoding variant; the argument is rewritten as `getFPOffset(arg1, dataOffset)` so the emitted displacement is the actual frame-relative byte offset.

```asm
  mov  eax, dword ptr [rbp+__arg32_1]
  add  edx, eax
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %09Dh`

```asm
  mov  eax, [ebp+__arg32_1]
  add  edx, eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %9Dh`

```asm
  add     x11, x29, __arg12_1
  ldrsw   x12,  [x11]
  add     x9, x9, x12
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %9Dh`

```asm
  addi    r16, r31, __arg16_1
  lwz     r16, 0(r16)
  add     r14, r14, r16
```

## Notes
- 32-bit add of a frame slot into `index`; for 64-bit see `xladddp` (0xBD).
- Argument is the abstract dp index; JIT rewrites it via `getFPOffset` before emit.
- Sign-extends the loaded 32-bit slot to 64 bits on amd64/aarch64 (`movsxd` / `ldrsw`).
- Preserves `acc`, `sp[0..1]`; writes `index` and condition flags.
