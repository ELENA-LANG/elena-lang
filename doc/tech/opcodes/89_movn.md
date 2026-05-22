# 0x89 -- mov n

- **Category:** DoubleOp
- **Enum:** `ByteCode::MovN`
- **Operand(s):** `mov n:n` (or `mov arg:n` -- same emission)
- **Reads:** --
- **Writes:** `index`

## Semantics
`index := n` -- load a 32-bit immediate into the data accumulator. The `arg:n` form encodes packed `argCount`/flags but is emitted identically. For 64-bit constants the upper half is loaded separately via a paired `lsavedp` sequence.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %89h`

**Selection rule:** `retrieveICode(arg1)` picks the immediate-form variant by the literal value -- narrow constants get short forms (8-bit / 16-bit / sign-extended); wider negatives select the `%989h`-style MOVZ/MOVK pair on aarch64.

```asm
  mov  edx, __n_1
```

Implicit zero-extension to `rdx`.

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %89h`

```asm
  mov  edx, __n_1
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %89h`

```asm
  mov    x9,  __n16_1
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%89h`  | small unsigned `n` (fits MOV imm) | single `mov x9, n` |
| `%989h` | `n < 0` | MOVZ/MOVK 16-bit pair + `sxtw x9, w9` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %89h`

```asm
  li      r14, __n16_1
```

## Notes
- Same emission shape as `mov mssg` (0x88), but the immediate is a plain literal -- no relocation.
- For 64-bit constants the upper half is loaded separately via a paired `lsavedp` sequence.
- Variant slot is selected by literal value (`retrieveICode`).
- Preserves `acc`, `sp[0..1]`; only `index` is written.
