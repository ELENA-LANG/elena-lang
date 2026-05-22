# 0x82 -- nlen

- **Category:** DoubleOp
- **Enum:** `ByteCode::NLen`
- **Operand(s):** `nlen n`
- **Reads:** `acc`, header at `[acc - elSizeOffset]`
- **Writes:** `index`

## Semantics
`index := acc.length / n` -- read the raw size field from the object header (masked with `struct_mask_inv` to drop the struct flag) and divide by element size `n`. Specialised variants for `n = 1, 2, 4, 8` replace the divide with a shift.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %82h`

**Selection rule:** `retrieveICode(arg)` (`elenasrc3/engine/jitcompiler.cpp:2399`) picks the power-of-2 variant slot by element size: `n == 1 -> slot 1`, `n == 2 -> slot 2`, `n == 4 -> slot 3`, `n == 8 -> slot 4`, anything else -> slot 0 (generic `idiv`).

```asm
  mov  eax, struct_mask_inv
  and  eax, dword ptr [rbx-elSizeOffset]
  mov  ecx, __n_1
  cdq
  idiv ecx
  mov  rdx, rax
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%82h`  | arbitrary `n` | masked size / `n` via `idiv` |
| `%182h` | `n == 1` | masked size only (no shift) |
| `%282h` | `n == 2` | `shr edx, 1` |
| `%382h` | `n == 4` | `shr edx, 2` |
| `%482h` | `n == 8` | `shr edx, 3` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %82h`

```asm
  mov  eax, struct_mask_inv
  and  eax, [ebx-elSizeOffset]
  mov  ecx, __n_1
  cdq
  idiv ecx
  mov  edx, eax
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%82h`  | arbitrary `n` | masked size / `n` via `idiv` |
| `%182h` | `n == 1` | masked size only |
| `%282h` | `n == 2` | `shr edx, 1` |
| `%382h` | `n == 4` | `shr edx, 2` |
| `%482h` | `n == 8` | `shr edx, 3` |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %82h`

```asm
  mov     x18, __n16_1

  sub     x11, x10, elSizeOffset
  ldr     w9, [x11]
  and     x9, x9, struct_mask_inv
  sdiv    x17, x17, x18
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%82h`  | arbitrary `n` | `sdiv` by `n` |
| `%182h` | `n == 1` | masked size only |
| `%282h` | `n == 2` | `lsr x9, x9, #1` |
| `%382h` | `n == 4` | `lsr x9, x9, #2` |
| `%482h` | `n == 8` | `lsr x9, x9, #3` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %82h`

```asm
  li      r16, struct_mask_inv_lo
  addis   r16, r16, struct_mask_inv_hi

  li      r18, __n16_1

  lwz     r14, -elSizeOffset(r15)
  and     r14, r14, r16
  divw    r14, r14, r18
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%82h`  | arbitrary `n` | `divw` by `n` |
| `%182h` | `n == 1` | masked size only |
| `%282h` | `n == 2` | `srdi r14, r14, 1` |
| `%382h` | `n == 4` | `srdi r14, r14, 2` |
| `%482h` | `n == 8` | `srdi r14, r14, 3` |

## Notes
- Reads the raw size word from `[acc - elSizeOffset]` and masks it with `struct_mask_inv` (drops the struct flag).
- Power-of-2 `n` becomes a shift; arbitrary `n` falls back to signed `idiv` / `sdiv` / `divw`.
- Companion to `mlen` (0x15) which returns the raw size without dividing.
- Preserves `acc`, `sp[0..1]`; only `index` is written.
