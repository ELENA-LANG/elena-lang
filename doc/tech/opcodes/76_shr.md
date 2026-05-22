# 0x76 -- shr

- **Category:** DoubleOp
- **Enum:** `ByteCode::Shr`
- **Operand(s):** `n` (shift count)
- **Reads:** `index`
- **Writes:** `index`

## Semantics
Right shift of the integer index register: `index := index >> n`. The amd64/x32 backends emit `shr` (logical, zero-fill); aarch64 emits `lsr`; ppc64le emits `srd`. Special-cased variants exist for `n=1,2,3`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %76h`

```asm
  mov  ecx, __n_1
  shr  edx, cl
```

**Selection rule** (`loadIOp` + `retrieveICode`, `jitcompiler.cpp:1086,2399`): the JIT picks the literal-shift variant when `arg1  in  {1, 2, 4, 8}`; otherwise the base variant emits the register-form shift.

### Variants
| Prefix | Selected when | Body |
|---|---|---|
| `%076h` | base (n variable) | `mov ecx, n; shr edx, cl` |
| `%276h` | n = 1 | `shr edx, 1` |
| `%376h` | n = 2 | `shr edx, 2` |
| `%476h` | n = 3 | `shr edx, 3` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %76h`

```asm
  mov  ecx, __n_1
  shr  edx, cl
```

### Variants
| Prefix | Selected when | Body |
|---|---|---|
| `%076h` | base | `mov ecx, n; shr edx, cl` |
| `%276h` | n = 1 | `shr edx, 1` |
| `%376h` | n = 2 | `shr edx, 2` |
| `%476h` | n = 3 | `shr edx, 3` |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %76h`

```asm
  mov     x18, __n16_1
  lsr     x9, x9, x18
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %76h`

```asm
  li      r18, __n16_1
  srd     r14, r14, r18
```

## Notes
- **Logical** right shift (zero-fill), not arithmetic -- amd64/x32 use `shr`, aarch64 `lsr`, ppc64le `srd`. Negative values become large positives.
- For arithmetic (sign-preserving) right shift, a different opcode would be required; this one drops the sign bit.
- JIT selects the literal-shift variant for `n  in  {1, 2, 3}`; falls back to the register-form (`mov cl, n; shr edx, cl`) otherwise.
- Pairs with `shl` (0x75) for bit-manipulation patterns.
- `acc` and stack are preserved; only `index` is mutated.
