# 0xC6 -- xcmp sp

- **Category:** DoubleOp
- **Enum:** `ByteCode::XCmpSI`
- **Operand(s):** `xcmp sp:i`
- **Reads:** `index`, `sp[i]`
- **Writes:** `COMP.EQ`, `COMP.LT`

## Semantics
`COMP.EQ := (index == sp[i])`; `COMP.LT := (index < sp[i])` -- signed compare of the data accumulator against a stack slot.

## JIT (amd64)
**Selection rule:** dispatched through `loadStackIndexOp` -> `retrieveCode` with the displacement transformed via `stackOffset + arg`. The picker keys on the resulting slot index -- `i == 0` (slot 1) and `i == 1` (slot 2) get peephole variants using the shadow registers `r10`/`r11`; all other values fall through to the generic `cmp rdx, [rsp + i]`.

**Template:** `cmp rdx, [rsp + i]`; peephole forms for `i == 0/1` use the shadow regs `r10/r11`.

```asm
cmp rdx, qword ptr [rsp + __arg32_1]
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0C6h` | generic `sp:i` | `cmp rdx, qword ptr [rsp + __arg32_1]` |
| `%1C6h` | `sp:0` | `cmp rdx, r10` |
| `%2C6h` | `sp:1` | `cmp rdx, r11` |

## JIT (x32)
**Template:** materialise the slot into `eax` then `cmp edx, eax`; peephole for `sp:0` (shadow `esi`).

```asm
mov  eax, [esp + __arg32_1]
cmp  edx, eax
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0C6h` | generic `sp:i` | `mov eax, [esp+i]; cmp edx, eax` |
| `%1C6h` | `sp:0` | `cmp edx, esi` |

## JIT (aarch64)
**Template:** load via `sp + imm` into x11, `cmp x9, x11`; peepholes use x0/x1.

```asm
add     x11, sp, __arg12_1
ldr     x11, [x11]
cmp     x9, x11
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0C6h` | generic | `add x11, sp, i; ldr x11,[x11]; cmp x9, x11` |
| `%1C6h` | `sp:0` | `mov x11, x0; cmp x9, x11` |
| `%2C6h` | `sp:1` | `mov x11, x1; cmp x9, x11` |

## JIT (ppc64le)
**Template:** `ld` from `r1 + imm`, `cmpd` against `r14` (index).

```asm
ld      r16, __arg16_1(r1)
cmpd    r14, r16
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0C6h` | generic | `ld r16, i(r1); cmpd r14, r16` |
| `%1C6h` | `sp:0` | `cmpd r14, r3` |
| `%2C6h` | `sp:1` | `cmpd r14, r4` |

## Notes
- Signed pointer-width compare (`cmpd` on ppc64le, otherwise full-width `cmp`).
- Compare with `cmp sp:i` (0xC9) which targets `acc` instead of `index`.
- The JIT applies a `stackOffset + arg` transform before slot selection -- peephole variants for `sp:0`/`sp:1` use shadow registers that mirror the top of the managed stack.
- COMP flags are the only outputs; both `acc` and `index` are preserved.
