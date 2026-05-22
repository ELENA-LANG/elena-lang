# 0xA2 -- save sp

- **Category:** DoubleOp
- **Enum:** `ByteCode::SaveSI`
- **Operand(s):** `save sp:i`
- **Reads:** `index`
- **Writes:** `sp[i]` (cached register for i=0/1, memory otherwise)
- **Side effects:** None.

## Semantics
`sp[i] := index` -- store the index register into the stack slot at offset `i`. The top one or two stack slots are register-cached; the JIT picks a specialized variant when `i = 0` or `i = 1`.

## JIT (amd64)

**Selection rule:** dispatched through `loadStackIndexOp` (`jitcompiler.cpp:898-907`) -- `retrieveCode(arg1)` selects the inline variant (`%0A2h` for generic, `%1A2h` for `sp:0`, `%2A2h` for `sp:1`) and the operand slot is rewritten as `(scope->stackOffset + arg1) << indexPower`. The JIT keeps `stackOffset` updated on every `alloc`/`free`/frame-changing opcode so the bytecode's *logical* stack index lands on the right physical SP-relative byte offset.

**Template:** `asm/amd64/core60.asm` `inline %0A2h`

```asm
  mov  eax, edx
  mov  [rsp + __arg32_1], rax
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A2h` | generic `sp:i` (i > 1) | `mov eax, edx; mov [rsp+i], rax` |
| `%1A2h` | `sp:0`                 | `mov r10, rdx` |
| `%2A2h` | `sp:1`                 | `mov r11, rdx` |

The generic path goes through `eax` first, zero-extending the low 32 bits of `index` before the 64-bit store. The full-word companion is `lsave sp:disp` (0xAB).

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0A2h`

```asm
  mov [esp + __arg32_1], edx
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A2h` | generic `sp:i` (i > 0) | `mov [esp+i], edx` |
| `%1A2h` | `sp:0`                 | `mov esi, edx` |

x32 caches only `sp[0]` (in `esi`); no `sp:1` variant.

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0A2h`

```asm
  add     x11, sp, __arg12_1
  str     x9, [x11]
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A2h` | generic `sp:i` (i > 1) | `add x11, sp, i; str x9, [x11]` |
| `%1A2h` | `sp:0`                 | `mov x0, x9` |
| `%2A2h` | `sp:1`                 | `mov x1, x9` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0A2h`

```asm
  std     r14, __arg16_1(r1)
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0A2h` | generic `sp:i` (i > 1) | `std r14, i(r1)` |
| `%1A2h` | `sp:0`                 | `mr r3, r14` |
| `%2A2h` | `sp:1`                 | `mr r4, r14` |

## Notes

- Truncating 32-bit store of `index` into stack slot `i` (low half only on amd64 -- goes via `eax`).
- Top-of-stack slots (`sp:0`, `sp:1`) are register-cached: amd64 in `r10`/`r11`, x32 only `sp:0` in `esi`, aarch64 in `x0`/`x1`, ppc64le in `r3`/`r4`.
- 64-bit companion is `lsave sp:disp` (0xAB).
- Bytecode index is translated by the JIT: `physical_disp = (stackOffset + i) << indexPower`.
