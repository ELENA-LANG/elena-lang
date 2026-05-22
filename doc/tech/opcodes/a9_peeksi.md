# 0xA9 -- peek sp

- **Category:** DoubleOp
- **Enum:** `ByteCode::PeekSI`
- **Operand(s):** `peek sp:i`
- **Reads:** `sp[i]` (cached register for i=0/1, memory otherwise)
- **Writes:** `acc`
- **Side effects:** None.

## Semantics
`acc := sp[i]` -- load stack slot `i` into `acc`. The top one or two slots are register-cached. Distinct from `set sp:i` (0xAF), which computes the *address* of the slot via `lea`.

## JIT (amd64)

**Selection rule:** dispatched through `loadStackIndexOp` (`jitcompiler.cpp:898-907`) -- `retrieveCode(arg1)` picks the variant (`%0A9h` generic, `%1A9h` for `sp:0`, `%2A9h` for `sp:1`) and the operand slot is rewritten as `(scope->stackOffset + arg1) << indexPower`. Cached slots short-circuit the memory read into a register-to-register move.

**Template:** `asm/amd64/core60.asm` `inline %0A9h`

```asm
  mov rbx, qword ptr [rsp + __arg32_1]
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A9h` | generic `sp:i` (i > 1) | `mov rbx, [rsp+i]` |
| `%1A9h` | `sp:0`                 | `mov rbx, r10` |
| `%2A9h` | `sp:1`                 | `mov rbx, r11` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0A9h`

```asm
  mov ebx, [esp + __arg32_1]
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A9h` | generic `sp:i` (i > 0) | `mov ebx, [esp+i]` |
| `%1A9h` | `sp:0`                 | `mov ebx, esi` |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0A9h`

```asm
  add     x11, sp, __arg12_1
  ldr     x10, [x11]
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A9h` | generic `sp:i` (i > 1) | `add x11, sp, i; ldr x10, [x11]` |
| `%1A9h` | `sp:0`                 | `mov x10, x0` |
| `%2A9h` | `sp:1`                 | `mov x10, x1` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0A9h`

```asm
  ld      r15, __arg16_1(r1)
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A9h` | generic `sp:i` (i > 1) | `ld r15, i(r1)` |
| `%1A9h` | `sp:0`                 | `mr r15, r3` |
| `%2A9h` | `sp:1`                 | `mr r15, r4` |

## Notes

- Loads `sp[i]` into `acc`; distinct from `set sp:i` (0xAF) which loads the *address* of the slot via `lea`.
- Cached top-of-stack slots avoid memory traffic entirely (amd64 `r10`/`r11`, x32 `esi`, aarch64 `x0`/`x1`, ppc64le `r3`/`r4`).
- Inverse of `store sp:i` (0xA3).
- Bytecode index translated to physical SP offset by `(stackOffset + i) << indexPower`.
