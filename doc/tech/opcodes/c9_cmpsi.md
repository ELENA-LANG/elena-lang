# 0xC9 -- cmp sp

- **Category:** DoubleOp
- **Enum:** `ByteCode::CmpSI`
- **Operand(s):** `cmp sp:i`
- **Reads:** `acc`, `sp[i]`
- **Writes:** `COMP.EQ`, `COMP.LT`

## Semantics
`COMP.EQ := (acc == sp[i])`; `COMP.LT := (acc < sp[i])` -- signed pointer-width compare of the accumulator against a stack slot.

## JIT (amd64)
**Selection rule:** dispatched through `loadStackIndexOp` -> `retrieveCode` with the displacement transformed via `stackOffset + arg`. The slot picker yields peephole variants for `i == 0` (slot 1) and `i == 1` (slot 2) which use the shadow registers `r10`/`r11`; all other values fall through to the generic `cmp rbx, [rsp + i]`.

**Template:** memory operand on a 64-bit `cmp`; peepholes for sp:0/sp:1 use `r10`/`r11`.

```asm
cmp rbx, qword ptr [rsp + __arg32_1]
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0C9h` | generic `sp:i` | `cmp rbx, qword ptr [rsp + i]` |
| `%1C9h` | `sp:0` | `cmp rbx, r10` |
| `%2C9h` | `sp:1` | `cmp rbx, r11` |

## JIT (x32)
**Template:** materialise the slot, then `cmp ebx, eax`; peephole for `sp:0` (`esi`).

```asm
mov  eax, [esp + __arg32_1]
cmp  ebx, eax
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0C9h` | generic | `mov eax, [esp+i]; cmp ebx, eax` |
| `%1C9h` | `sp:0` | `cmp ebx, esi` |

## JIT (aarch64)
**Template:** load into x11, `cmp x10, x11`; peepholes use x0/x1.

```asm
add     x11, sp, __arg12_1
ldr     x11, [x11]
cmp     x10, x11
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0C9h` | generic | `add x11, sp, i; ldr x11,[x11]; cmp x10, x11` |
| `%1C9h` | `sp:0` | `mov x11, x0; cmp x10, x11` |
| `%2C9h` | `sp:1` | `mov x11, x1; cmp x10, x11` |

## JIT (ppc64le)
**Template:** `ld` from `r1+i`, `cmpd r15, r16`; peepholes via r3/r4.

```asm
ld      r16, __arg16_1(r1)
cmpd    r15, r16
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0C9h` | generic | `ld r16, i(r1); cmpd r15, r16` |
| `%1C9h` | `sp:0` | `cmpd r15, r3` |
| `%2C9h` | `sp:1` | `cmpd r15, r4` |

## Notes
- Sibling of `xcmp sp:i` (0xC6) -- both target a stack slot, but `cmp sp` reads `acc` whereas `xcmp sp` reads `index`.
- Signed pointer-width compare.
- Slot selection applies `stackOffset + arg` so peephole variants for `sp:0`/`sp:1` map to shadow registers (`r10`/`r11`, `esi`, `x0/x1`, `r3/r4`) rather than memory.
- Both `acc` and `index` are preserved; only COMP flags are written.
