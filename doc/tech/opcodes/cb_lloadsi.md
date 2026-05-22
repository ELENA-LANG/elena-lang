# 0xCB -- lload sp

- **Category:** DoubleOp
- **Enum:** `ByteCode::LLoadSI`
- **Operand(s):** `lload sp:i`
- **Reads:** `sp[i]`
- **Writes:** `long:index`

## Semantics
`long:index := sp[i]` -- load a full pointer-width word from the stack into the data accumulator.

## JIT (amd64)
**Selection rule:** dispatched through `loadStackIndexOp` -> `retrieveCode` with the displacement transformed via `stackOffset + arg`. Slots 1/2 (i = 0/1) emit peephole register-to-register moves from the shadow registers `r10`/`r11`; all other indices fall through to the generic memory load.

**Template:** 64-bit load into `rdx`.

```asm
mov rdx, [rsp + __arg32_1]
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0CBh` | generic | `mov rdx, [rsp + i]` |
| `%1CBh` | `sp:0` | `mov rdx, r10` |
| `%2CBh` | `sp:1` | `mov rdx, r11` |

## JIT (x32)
**Template:** split into low/high 32-bit halves: `eax` low, `edx` high (long:index is the pair).

```asm
lea  edi, [esp + __arg32_1]
mov  eax, [edi]
mov  edx, [edi + 4]
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0CBh` | generic | load 8 bytes into `eax:edx` |
| `%1CBh` | `sp:0` | `mov eax, esi; xor edx, edx` (sp[0] shadow holds only the low 32 bits) |

## JIT (aarch64)
**Template:** `ldr` 64-bit into `x9`.

```asm
add     x11, sp, __arg12_1
ldr     x9, [x11]
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0CBh` | generic | `add x11, sp, i; ldr x9, [x11]` |
| `%1CBh` | `sp:0` | `mov x9, x0` |
| `%2CBh` | `sp:1` | `mov x9, x1` |

## JIT (ppc64le)
**Template:** 64-bit `ld` into `r14`.

```asm
ld      r14, __arg16_1(r1)
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0CBh` | generic | `ld r14, i(r1)` |
| `%1CBh` | `sp:0` | `mr r14, r3` |
| `%2CBh` | `sp:1` | `mr r14, r4` |

## Notes
- Counterpart of `load sp:i` (0xCC) which is the 32-bit signed-extending variant.
- x32 needs the high half explicitly because `long:index` is a 64-bit register pair (`eax:edx`); the `sp:0` peephole zero-extends since the shadow holds only the low 32 bits.
- Slot selection applies `stackOffset + arg` so `sp:0`/`sp:1` map to shadow registers rather than memory.
- Clobbers `index` (or the `eax:edx` pair on x32); no flags are touched.
