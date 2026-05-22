# 0x94 -- and n

- **Category:** DoubleOp
- **Enum:** `ByteCode::AndN`
- **Operand(s):** `andn n`
- **Reads:** `index`
- **Writes:** `index`, condition flags
- **Side effects:** 32-bit AND on x86 clears the upper half of `rdx`.

## Semantics
`index := index & n` -- bitwise AND of the data accumulator with a 32-bit immediate.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %94h`

**Selection rule:** `retrieveICode(arg1)` picks the immediate-form variant by literal value -- narrow constants get compact encodings; wider values dispatch to MOVZ/MOVK-pair variants on aarch64.

```asm
  and  edx, __n_1
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %94h`

```asm
  and  edx, __n_1
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %94h`

```asm
  mov     x19, __n16_1   ; temporally
  and     x9, x9, x19
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%94h`   | small immediate `n` fits MOV imm | `mov` + `and` |
| `%0994h` | wider immediate (signed) | MOVZ/MOVK 16-bit pair + `and` |
| `%0A94h` | wider immediate (unsigned high half) | MOVZ/MOVK 16-bit pair + `and` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %94h`

```asm
  andi.   r14, r14, __n16_1
```

## Notes
- Counterpart to `orn` (0x9B); both leave flags valid for the next compare/branch.
- 32-bit AND on x86 zeroes the upper half of `rdx` implicitly.
- Variant slot selected by literal value (`retrieveICode`).
- Preserves `acc`, `sp[0..1]`; writes `index` and condition flags.
