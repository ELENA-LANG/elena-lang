# 0x8C -- sub n

- **Category:** DoubleOp
- **Enum:** `ByteCode::SubN`
- **Operand(s):** `subn n`
- **Reads:** `index`
- **Writes:** `index`, condition flags
- **Side effects:** Sets architecture condition codes.

## Semantics
`index := index - n` -- subtract immediate from the data accumulator. Condition flags reflect the subtraction; subsequent compare/branch opcodes may rely on them.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %8Ch`

**Selection rule:** `retrieveICode(arg1)` picks the immediate-form variant by literal value -- narrow constants use compact encodings; wider / negative values may dispatch to MOVZ/MOVK-pair variants on aarch64.

```asm
  sub  rdx, __n_1
```

64-bit subtract with a sign-extended 32-bit immediate.

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %8Ch`

```asm
  sub  edx, __n_1
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %8Ch`

```asm
  mov    x11,  __n16_1
  sub    x9, x9, x11
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %8Ch`

```asm
  li      r18, __n16_1
  subf    r14, r18, r14
```

## Notes
- Counterpart to `addn` (0x8D); subsequent `jeq`/`jlt`/etc. read the flags.
- 32-bit operation on x86; aarch64/ppc64le widen the immediate first into a scratch register.
- Variant slot selected by literal value (`retrieveICode`).
- Preserves `acc`, `sp[0..1]`; writes `index` and condition flags.
