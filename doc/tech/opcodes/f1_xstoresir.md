# 0xF1 -- xstore sp

- **Category:** TripleOp
- **Enum:** `ByteCode::XStoreSIR`
- **Operand(s):** `xstore sp:i, r` (R-cmd)

## Semantics
`sp[i] := r` -- write the immediate reference `r` into a stack slot (or into the shadowed sp[0]/sp[1] register on architectures where those slots live in registers).

## JIT (amd64)
**Selection rule:** R-cmd -- variant by `mskXxx` class of the stored ref combined with `(i, r)` shape. The argument is rewritten as `stackOffset + arg`, so the effective slot index already includes any outer-frame adjustment. Sentinel values (`r == 0`, `r == -1`) and shadow-register slots (i=0/i=1) get dedicated fast slots.

**Template:** `asm/amd64/core60.asm` `inline %0F1h`
```asm
; generic
mov  rax, __ptr64_2
mov  qword ptr [rsp+__arg32_1], rax
```

### Variants
| Prefix | (i, r) | Body |
|---|---|---|
| `%0F1h` | generic | `mov qword [rsp+i], r` |
| `%1F1h` | (0, r) | `mov r10, r` (sp[0] shadow) |
| `%2F1h` | (1, r) | `mov r11, r` (sp[1] shadow) |
| `%6F1h` | (0, 0) | `mov r10, 0` |
| `%7F1h` | (1, 0) | `mov r11, 0` |
| `%9F1h` | (0, -1) | `mov r10, -1` |
| `%0AF1h` | (1, -1) | `mov r11, -1` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0F1h`
```asm
; generic
mov  eax, __ptr32_2
mov  [esp+__arg32_1], eax
```

### Variants
| Prefix | (i, r) | Body |
|---|---|---|
| `%0F1h` | generic | `mov [esp+i], r` |
| `%1F1h` | (0, r) | `mov esi, r` (sp[0] shadow) |
| `%6F1h` | (0, 0) | `mov esi, 0` |
| `%9F1h` | (0, -1) | `mov esi, r` (= -1 ref) |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0F1h`
```asm
; generic
movz    x11, __ptr32lo_2
movk    x11, __ptr32hi_2, lsl #16

add     x12, sp, __arg12_1
str     x11, [x12]
```

### Variants
| Prefix | (i, r) | Body |
|---|---|---|
| `%0F1h` | generic | `str x11, [sp+i]` |
| `%1F1h` | (0, r) | `mov x0, r` (sp[0]) |
| `%2F1h` | (1, r) | `mov x1, r` (sp[1]) |
| `%5F1h` | (i, 0) | `str xzr-style, [sp+i]` |
| `%6F1h` | (0, 0) | `mov x0, 0` |
| `%7F1h` | (1, 0) | `mov x1, 0` |
| `%8F1h` | (i, -1) | `str -1, [sp+i]` |
| `%9F1h` | (0, -1) | `mov x0, -1` |
| `%0AF1h` | (1, -1) | `mov x1, -1` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0F1h`
```asm
; generic
ld      r16, toc_code(r2)
addis   r16, r16, __xdisp32hi_2
addi    r16, r16, __xdisp32lo_2

std     r16, __arg16_1(r1)
```

### Variants
| Prefix | (i, r) | Body |
|---|---|---|
| `%0F1h` | generic | `std r16, [sp+i]` |
| `%1F1h` | (0, r) | `mr r3, r` (sp[0]) |
| `%2F1h` | (1, r) | `mr r4, r` (sp[1]) |
| `%5F1h` | (i, 0) | `li r16,0; std r16, [sp+i]` |
| `%6F1h` | (0, 0) | `li r3, 0` |
| `%7F1h` | (1, 0) | `li r4, 0` |
| `%8F1h` | (i, -1) | `li/addi -1; std [sp+i]` |
| `%9F1h` | (0, -1) | `li r3, 0; addi r3, r3, -1` |
| `%0AF1h` | (1, -1) | `li r4, 0; addi r4, r4, -1` |

## Notes
- Writes the immediate ref `r` directly into a stack slot or its shadow register.
- Shadow register slots (sp[0] -> `r10`/`esi`/`x0`/`r3`, sp[1] -> `r11`/`x1`/`r4`) bypass memory entirely.
- Sentinel `r == 0` and `r == -1` get dedicated immediate-load slots so the JIT need not emit a full ptr-load sequence.
- `arg1` is rewritten as `stackOffset + arg` at emit time -- outer-frame shifts are folded into the displacement.
