# 0x1B -- convl

- **Category:** SingleOp
- **Enum:** `ByteCode::ConvL`
- **Operand(s):** (none)
- **Reads:** `index` (low 32 bits)
- **Writes:** `long:index`

## Semantics
Sign-extends the low 32 bits of `index` to 64 bits. Used to promote an int32 result before a long operation or 64-bit comparison. On x32 the result is the `eax:edx` pair.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %1Bh`

```asm
  movsxd rdx, edx
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %1Bh`

```asm
  mov  eax, edx
  cdq
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %1Bh`

```asm
  sxtw    x9, w9
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %1Bh`

```asm
  extsw   r14, r14
```

## Notes
- Sign-extends the low 32 bits of `index` to the full register width. Used to promote an int32 result before a long operation or 64-bit compare.
- Per-arch instruction names differ: amd64 `movsxd rdx, edx`; x32 `cdq` (writes `edx` as the high half of `eax:edx`); aarch64 `sxtw x9, w9`; ppc64le `extsw r14, r14`.
- Preserves `acc`, `sp[0]`, `sp[1]`. Clobbers only `index` (on x32, both halves of the pair).
