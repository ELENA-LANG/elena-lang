# 0x8D -- add n

- **Category:** DoubleOp
- **Enum:** `ByteCode::AddN`
- **Operand(s):** `addn n`
- **Reads:** `index`
- **Writes:** `index`, condition flags
- **Side effects:** Sets architecture condition codes.

## Semantics
`index := index + n` -- add immediate to the data accumulator.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %8Dh`

**Selection rule:** `retrieveICode(arg1)` picks the immediate-form variant by literal value -- narrow constants use compact encodings; wider values dispatch to MOVZ/MOVK-pair variants on aarch64.

```asm
  add  rdx, __n_1
```

64-bit add with a sign-extended 32-bit immediate.

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %8Dh`

```asm
  add  edx, __n_1
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %8Dh`

```asm
  mov    x11,  __n16_1
  add    x9, x9, x11
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%8Dh`  | small immediate `n` fits MOV imm | `mov` + `add` |
| `%98Dh` | wider immediate | MOVZ/MOVK 16-bit pair + `sxtw` + `add` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %8Dh`

```asm
  li      r18, __n16_1
  add     r14, r14, r18
```

## Notes
- Counterpart to `subn` (0x8C); flags are reusable by the next compare/branch.
- 32-bit add on x86; aarch64/ppc64le widen the immediate first into a scratch register.
- Variant slot selected by literal value (`retrieveICode`).
- Preserves `acc`, `sp[0..1]`; writes `index` and condition flags.
