# 0xA3 -- store sp

- **Category:** DoubleOp
- **Enum:** `ByteCode::StoreSI`
- **Operand(s):** `store sp:i`
- **Reads:** `acc`
- **Writes:** `sp[i]` (cached register for i=0/1, memory otherwise)
- **Side effects:** None -- the stack is GC-scanned, so no write barrier.

## Semantics
`sp[i] := acc` -- spill `acc` into the stack slot at offset `i`. Inverse of `peek sp:i` (0xA9). The top one or two stack slots are register-cached; the JIT picks a specialized variant when `i = 0` or `i = 1`.

## JIT (amd64)

**Selection rule:** dispatched through `loadStackIndexOp` (`jitcompiler.cpp:898-907`) -- `retrieveCode(arg1)` selects the inline variant (`%0A3h` generic, `%1A3h` for `sp:0`, `%2A3h` for `sp:1`) and the operand slot is rewritten as `(scope->stackOffset + arg1) << indexPower`. The cached top-of-stack slots avoid a memory access entirely.

**Template:** `asm/amd64/core60.asm` `inline %0A3h`

```asm
  mov qword ptr [rsp + __arg32_1], rbx
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A3h` | generic `sp:i` (i > 1) | `mov [rsp+i], rbx` |
| `%1A3h` | `sp:0`                 | `mov r10, rbx` |
| `%2A3h` | `sp:1`                 | `mov r11, rbx` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0A3h`

```asm
  mov [esp + __arg32_1], ebx
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A3h` | generic `sp:i` (i > 0) | `mov [esp+i], ebx` |
| `%1A3h` | `sp:0`                 | `mov esi, ebx` |

x32 caches only `sp[0]` (in `esi`); no `sp:1` variant.

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0A3h`

```asm
  add     x11, sp, __arg12_1
  str     x10, [x11]
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A3h` | generic `sp:i` (i > 1) | `add x11, sp, i; str x10, [x11]` |
| `%1A3h` | `sp:0`                 | `mov x0, x10` |
| `%2A3h` | `sp:1`                 | `mov x1, x10` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0A3h`

```asm
  std     r15, __arg16_1(r1)
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A3h` | generic `sp:i` (i > 1) | `std r15, i(r1)` |
| `%1A3h` | `sp:0`                 | `mr r3, r15` |
| `%2A3h` | `sp:1`                 | `mr r4, r15` |

## Notes

- Spill of `acc` into `sp[i]`, the natural argument-passing/temporary slot.
- The stack is GC-scanned, so no write barrier is needed; contrast with `assigni` (0xA6) which has one because objects can be in older generations.
- Inverse of `peek sp:i` (0xA9).
- Cached slots: amd64 `r10`/`r11`, x32 `esi` (only `sp:0`), aarch64 `x0`/`x1`, ppc64le `r3`/`r4`.
