# 0x87 -- swap sp

- **Category:** DoubleOp
- **Enum:** `ByteCode::SwapSI`
- **Operand(s):** `swap sp:i`
- **Reads:** `acc`, `sp[i]`
- **Writes:** `acc`, `sp[i]`

## Semantics
`acc <=> sp[i]` -- swap the accumulator with stack slot `i`. Specialised variants use cached `sp[0]` / `sp[1]` registers to avoid memory access.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %87h`

**Selection rule:** `retrieveCode(arg1)` picks the variant slot by stack-index magnitude -- small indices (0 and 1) have dedicated short-form bodies that swap with the cached `sp[0]` / `sp[1]` registers and avoid memory access.

```asm
  mov  rax, [rsp+__arg32_1]
  mov  [rsp+__arg32_1], rbx
  mov  rbx, rax
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%87h`  | `i` not in {0, 1} | three-mov memory swap above |
| `%187h` | `i == 0` | register-only swap between `rbx` and `r10` |
| `%287h` | `i == 1` | register-only swap between `rbx` and `r11` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %87h`

```asm
  mov  eax, [esp+__arg32_1]
  mov  [esp+__arg32_1], ebx
  mov  ebx, eax
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%87h`  | `i != 0` | memory swap above |
| `%187h` | `i == 0` | register-only swap between `ebx` and cached `esi` |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %87h`

```asm
  mov     x13, x10
  add     x12, sp, __arg12_1
  ldr     x10, [x12]
  str     x13, [x12]
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%87h`  | `i` not in {0, 1} | memory swap above |
| `%187h` | `i == 0` | register-only swap between `x10` and `x0` via `x13` |
| `%287h` | `i == 1` | register-only swap between `x10` and `x1` via `x13` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %87h`

```asm
  mr       r16, r15
  ld       r15, __arg16_1(r1)
  std      r16, __arg16_1(r1)
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%87h`  | `i` not in {0, 1} | memory swap above |
| `%187h` | `i == 0` | register-only swap between `r15` and `r3` via `r16` |
| `%287h` | `i == 1` | register-only swap between `r15` and `r4` via `r16` |

## Notes
- Swaps `acc` with `sp[i]`; compare `xswap sp:i` (0x86) which swaps `sp[0]` with `sp[i]`.
- `i == 0` and `i == 1` short forms reuse the cached `sp[0]` / `sp[1]` registers -- no memory traffic.
- On x32 only `sp[0]` is cached (`esi`), so only the `i == 0` short form exists.
- Preserves `index` and all other stack slots.
