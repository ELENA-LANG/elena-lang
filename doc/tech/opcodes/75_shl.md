# 0x75 -- shl

- **Category:** DoubleOp
- **Enum:** `ByteCode::Shl`
- **Operand(s):** `n` (shift count)
- **Reads:** `index`
- **Writes:** `index`

## Semantics
Logical left shift of the integer index register: `index := index << n`. The shift amount `n` is encoded as an immediate operand. Special-cased variants exist for `n=1,2,3` selecting fixed-shift encodings.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %75h`

```asm
  mov  ecx, __n_1
  shl  edx, cl
```

**Selection rule** (`loadIOp` + `retrieveICode`, `jitcompiler.cpp:1086,2399`): the JIT picks the literal-shift variant when `arg1  in  {1, 2, 4, 8}`; otherwise the base variant emits the register-form shift.

### Variants
| Prefix | Selected when | Body |
|---|---|---|
| `%075h` | base (n variable) | `mov ecx, n; shl edx, cl` |
| `%275h` | n = 1 | `shl edx, 1` |
| `%375h` | n = 2 | `shl edx, 2` |
| `%475h` | n = 3 | `shl edx, 3` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %75h`

```asm
  mov  ecx, __n_1
  shl  edx, cl
```

### Variants
| Prefix | Selected when | Body |
|---|---|---|
| `%075h` | base | `mov ecx, n; shl edx, cl` |
| `%275h` | n = 1 | `shl edx, 1` |
| `%375h` | n = 2 | `shl edx, 2` |
| `%475h` | n = 3 | `shl edx, 3` |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %75h`

```asm
  mov     x18, __n16_1
  lsl     x9, x9, x18
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %75h`

```asm
  li      r18, __n16_1
  sld     r14, r14, r18
```

## Notes
- Logical left shift on `index` by an immediate count `n`.
- JIT picks the immediate-form variants for `n  in  {1, 2, 3}` (and via `retrieveICode` for `n  in  {1,2,4,8}` selection) to dodge the `mov cl, n` setup on x86.
- aarch64/ppc64le always materialize `n` into a GPR before shifting because their shift insns take register operands.
- Pairs naturally with `shr` (0x76) for bit-field extract / pack idioms.
- `acc` and stack are preserved; only `index` is mutated. x86 leaves CF set from the shift -- do not rely on it.
