# 0x86 -- xswap sp

- **Category:** DoubleOp
- **Enum:** `ByteCode::XSwapSI`
- **Operand(s):** `xswap sp:i`
- **Reads:** `sp[0]`, `sp[i]`
- **Writes:** `sp[0]`, `sp[i]`

## Semantics
`sp[0] <=> sp[i]` -- swap the contents of stack slot 0 with stack slot `i`. Specialised variants use cached `sp[0]` / `sp[1]` registers to avoid memory access. Compare with `swap sp:i` (0x87) which swaps `acc` with `sp[i]`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %86h`

**Selection rule:** `retrieveCode(arg1)` picks the variant slot by stack-index magnitude -- small indices (0 and 1) have dedicated short-form bodies that avoid the memory swap by operating on cached `sp[0]` / `sp[1]` registers.

```asm
  mov  rax, [rsp+__arg32_1]
  mov  [rsp+__arg32_1], r10
  mov  r10, rax
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%86h`  | `i` not in {0, 1} | three-mov memory swap above |
| `%186h` | `i == 0` | empty (swap with self) |
| `%286h` | `i == 1` | register-only swap between `r10` and `r11` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %86h`

```asm
  mov  eax, [esp+__arg32_1]
  mov  [esp+__arg32_1], esi
  mov  esi, eax
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%86h`  | `i != 0` | three-mov memory swap above |
| `%186h` | `i == 0` | empty (swap with self) |

Only `sp[0]` is cached in x32 (in `esi`); `sp[1]` is not cached, so no `%286h` variant exists.

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %86h`

```asm
  mov     x13, x0
  add     x12, sp, __arg12_1
  ldr     x0, [x12]
  str     x13, [x12]
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%86h`  | `i` not in {0, 1} | scratch-via-memory swap above |
| `%186h` | `i == 0` | empty |
| `%286h` | `i == 1` | register swap between `x0` and `x1` via `x13` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %86h`

```asm
  mr       r16, r3
  ld       r3, __arg16_1(r1)
  std      r16, __arg16_1(r1)
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%86h`  | `i` not in {0, 1} | scratch-via-memory swap above |
| `%186h` | `i == 0` | empty |
| `%286h` | `i == 1` | register swap between `r3` and `r4` via `r16` |

## Notes
- Swaps `sp[0]` with `sp[i]`; the accumulator is untouched (compare `swap sp:i` (0x87) which swaps `acc` with `sp[i]`).
- `i == 0` collapses to a no-op (swap with self).
- `i == 1` uses cached `sp[0]` / `sp[1]` registers -- no memory access.
- On x32 only `sp[0]` is cached (`esi`), so no `i == 1` short form exists.
