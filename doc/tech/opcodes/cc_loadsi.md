# 0xCC -- load sp

- **Category:** DoubleOp
- **Enum:** `ByteCode::LoadSI`
- **Operand(s):** `load sp:i`
- **Reads:** `sp[i]` (32-bit)
- **Writes:** `index`

## Semantics
`index := sp[i]` -- load the low 32-bit word of a stack slot into the data accumulator, sign-extended on 64-bit hosts.

## JIT (amd64)
**Selection rule:** dispatched through `loadStackIndexOp` -> `retrieveCode` with the displacement transformed via `stackOffset + arg`. Slots 1/2 (i = 0/1) get peephole moves from `r10`/`r11`; all other values fall through to a `movsxd` from `[rsp + i]`.

**Template:** `movsxd` 32-bit slot into `rdx`.

```asm
movsxd rdx, dword ptr [rsp + __arg32_1]
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0CCh` | generic | `movsxd rdx, [rsp + i]` |
| `%1CCh` | `sp:0` | `mov rdx, r10` |
| `%2CCh` | `sp:1` | `mov rdx, r11` |

## JIT (x32)
**Template:** plain 32-bit move (`index` is `edx`).

```asm
mov edx, [esp + __arg32_1]
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0CCh` | generic | `mov edx, [esp + i]` |
| `%1CCh` | `sp:0` | `mov edx, esi` |

## JIT (aarch64)
**Template:** `ldrsw` (load + sign-extend 32->64) into `x9`.

```asm
add     x11, sp, __arg12_1
ldrsw   x9, [x11]
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0CCh` | generic | `add x11, sp, i; ldrsw x9, [x11]` |
| `%1CCh` | `sp:0` | `mov x9, x0` |
| `%2CCh` | `sp:1` | `mov x9, x1` |

## JIT (ppc64le)
**Template:** `lwz` (zero-extending 32->64) into `r14`.

```asm
lwz     r14, __arg16_1(r1)
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0CCh` | generic | `lwz r14, i(r1)` |
| `%1CCh` | `sp:0` | `mr r14, r3` |
| `%2CCh` | `sp:1` | `mr r14, r4` |

## Notes
- 32-bit load; the 64-bit counterpart is `lload sp:i` (0xCB).
- On amd64 (`movsxd`) and aarch64 (`ldrsw`) the load sign-extends to 64 bits; on ppc64le (`lwz`) it zero-extends -- callers that need a signed full-width view should use `lload` instead.
- Slot selection applies `stackOffset + arg` so `sp:0`/`sp:1` peephole through shadow registers.
- Writes only `index`; `acc` and flags are preserved.
