# 0x9B -- or n

- **Category:** DoubleOp
- **Enum:** `ByteCode::OrN`
- **Operand(s):** `orn n`
- **Reads:** `index`
- **Writes:** `index`, condition flags
- **Side effects:** 32-bit OR on x86 zeroes the upper half of `rdx`.

## Semantics
`index := index | n` -- bitwise OR of the data accumulator with a 32-bit immediate. Counterpart to `andn` (0x94).

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %9Bh`

**Selection rule:** `retrieveICode(arg1)` picks the immediate-form variant by literal value -- narrow constants use compact encodings; wider values dispatch to MOVZ/MOVK-pair variants on aarch64.

```asm
  or  edx, __n_1
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %9Bh`

```asm
  or  edx, __n_1
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %9Bh`

```asm
  mov     x11, __n16_1
  orr     x9, x9, x11
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%9Bh`   | small immediate fits MOV imm | `mov` + `orr` |
| `%099Bh` | wider immediate (signed) | MOVZ/MOVK 16-bit pair + `orr` |
| `%0A9Bh` | wider immediate (unsigned) | MOVZ/MOVK 16-bit pair + `orr` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %9Bh`

```asm
  ori     r14, r14, __n16_1
```

## Notes
- Counterpart to `andn` (0x94); both leave flags valid for the next compare/branch.
- 32-bit OR on x86 zeroes the upper half of `rdx` implicitly.
- Variant slot selected by literal value (`retrieveICode`).
- Preserves `acc`, `sp[0..1]`; writes `index` and condition flags.
