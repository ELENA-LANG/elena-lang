# 0x97 -- cmp n

- **Category:** DoubleOp
- **Enum:** `ByteCode::CmpN`
- **Operand(s):** `cmpn n`
- **Reads:** `index`
- **Writes:** condition flags (COMP.EQ, COMP.LT)
- **Side effects:** Sets architecture condition codes for the next `jeq`/`jne`/`jlt`/`jge`/etc.

## Semantics
`COMP.EQ := (index == n)` and `COMP.LT := (index < n)` -- signed 32-bit compare of the data accumulator against an immediate. For 64-bit values use `xlcmp` (0x1C) or `icmp n` (0xC2) with `n = 8`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %097h`

**Selection rule:** `retrieveICode(arg1)` picks the immediate-form variant by literal value -- narrow constants use compact encodings; wider / negative values dispatch to MOVZ/MOVK-pair (or `lis`/`addis`) variants on aarch64 / ppc64le.

```asm
  cmp  edx, __n_1
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %097h`

```asm
  cmp  edx, __n_1
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %97h`

```asm
  mov     x18, __n16_1
  cmp     x9, x18
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%97h`   | small unsigned `n` fits MOV imm | `mov` + `cmp` |
| `%0997h` | `n < 0` | MOVZ/MOVK + `sxtw` + `cmp` |
| `%0A97h` | wider unsigned `n` | MOVZ/MOVK 16-bit pair + `cmp` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %97h`

```asm
  li      r18, __n16_1

  cmp     r14, r18
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%97h`   | small `n` fits `li` immediate | `li` + `cmp` |
| `%0997h` | `n < 0` | `lis` + `li` + masked `andi.` + `add` + `cmp` |
| `%0A97h` | `n > 0x7FFF` | masked `andi.` + `addis` + `cmp` |

## Notes
- 32-bit signed compare; for 64-bit see `xlcmp` (0x1C) or `icmp n` (0xC2) with `n = 8`.
- Only condition flags are written -- `index`, `acc`, `sp[0..1]` all preserved.
- Variant slot picked by literal value (`retrieveICode`); negative and wide-positive ranges get separate slots.
- Subsequent `jeq`/`jne`/`jlt`/`jge` opcodes consume the resulting flags.
