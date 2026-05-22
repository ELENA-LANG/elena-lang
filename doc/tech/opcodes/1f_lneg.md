# 0x1F -- lneg

- **Category:** SingleOp
- **Enum:** `ByteCode::LNeg`
- **Operand(s):** (none)
- **Reads:** `long:index`
- **Writes:** `long:index`

## Semantics
`long:index := -long:index` -- two's-complement negation of the full 64-bit `index`. 32-bit form is `0x22 neg`. On x32 it is implemented as a paired-register two's-complement (`not` both halves, add 1 with carry).

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %1Fh`

```asm
  neg    rdx
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %1Fh`

```asm
  not    edx
  not    eax
  add    eax, 1
  adc    edx, 0
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %1Fh`

```asm
  mov    x17, 0
  sub    x9, x17, x9
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %1Fh`

```asm
  neg    r14, r14
```

## Notes
- 64-bit negate of `index`. 32-bit form is `neg` (0x22).
- amd64/ppc64le emit a single instruction; aarch64 synthesises with `mov x17, 0; sub x9, x17, x9` (no single-operand 64-bit negate). x32 emits a four-instruction two's-complement on the `edx:eax` pair (`not` both halves, add 1 with carry).
- Preserves `acc`, `sp[0]`, `sp[1]`. Clobbers only `index` (and a scratch on aarch64).
