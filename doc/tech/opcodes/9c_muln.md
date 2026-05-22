# 0x9C -- mul n

- **Category:** DoubleOp
- **Enum:** `ByteCode::MulN`
- **Operand(s):** `muln n`
- **Reads:** `index`
- **Writes:** `index`, condition flags
- **Side effects:** 32-bit multiplication on x86 -- overflow is silent and upper half of `rdx` is zeroed.

## Semantics
`index := index * n` -- signed multiply of the data accumulator by an immediate.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %9Ch`

**Selection rule:** `retrieveICode(arg1)` picks the immediate-form variant by literal value -- narrow constants use compact encodings; wider values dispatch to MOVZ/MOVK-pair variants on aarch64.

```asm
  mov   eax, __n_1
  imul  edx, eax
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %9Ch`

```asm
  mov  eax, __n_1
  imul  edx, eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %9Ch`

```asm
  mov     x11, __n16_1
  mul     x9, x9, x11
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%9Ch`   | small immediate fits MOV imm | `mov` + `mul` |
| `%099Ch` | wider immediate (signed) | MOVZ/MOVK 16-bit pair + `mul` |
| `%0A9Ch` | wider immediate (unsigned) | MOVZ/MOVK 16-bit pair + `mul` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %9Ch`

```asm
  li      r16, __n16_1
  mulld   r14, r14, r16
```

## Notes
- 32-bit signed multiply on x86 -- overflow is silent and `rdx` upper half is zeroed.
- aarch64 / ppc64le use 64-bit `mul` / `mulld` -- the operand is materialised into a scratch first.
- Variant slot selected by literal value (`retrieveICode`).
- Preserves `acc`, `sp[0..1]`; writes `index` and condition flags.
