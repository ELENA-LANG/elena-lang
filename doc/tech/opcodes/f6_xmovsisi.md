# 0xF6 -- xmov sp

- **Category:** TripleOp
- **Enum:** `ByteCode::XMovSISI`
- **Operand(s):** `xmov sp:i1, sp:i2`

## Semantics
`sp[i1] := sp[i2]` -- copy one stack slot into another. Specialized variants exist for the sp[0]/sp[1] shadow registers.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %0F6h`
```asm
; generic
mov  rax, [rsp+__arg32_2]
mov  [rsp+__arg32_1], rax
```

### Variants
| Prefix | (i1, i2) | Body |
|---|---|---|
| `%0F6h` | generic | `mov rax,[rsp+i2]; mov [rsp+i1],rax` |
| `%1F6h` | (0, n) | `mov r10, [rsp+i2]` |
| `%2F6h` | (n, 0) | `mov [rsp+i1], r10` |
| `%3F6h` | (1, n) | `mov r11, [rsp+i2]` |
| `%4F6h` | (n, 1) | `mov [rsp+i1], r11` |
| `%5F6h` | (0, 1) | `mov r10, r11` |
| `%6F6h` | (1, 0) | `mov r11, r10` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0F6h`
```asm
; generic
mov  eax, [esp+__arg32_2]
mov  [esp+__arg32_1], eax
```

### Variants
| Prefix | (i1, i2) | Body |
|---|---|---|
| `%0F6h` | generic | `mov [esp+i1], eax` |
| `%1F6h` | (0, n) | `mov esi, [esp+i2]` |
| `%2F6h` | (n, 0) | `mov [esp+i1], esi` |
| `%5F6h` | (0, 1) | `mov esi, [esp+4]` |
| `%6F6h` | (1, 0) | `mov [esp+i1], esi` |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0F6h`
```asm
; generic
add     x12, sp, __arg12_1
add     x13, sp, __arg12_2

ldr     x11, [x12]
str     x11, [x13]
```

### Variants
| Prefix | (i1, i2) | Body |
|---|---|---|
| `%0F6h` | generic | `ldr/str` |
| `%1F6h` | (1, n) -> x0 | `ldr x0, [sp+i2]` |
| `%2F6h` | (n, 1) | `str x0, [sp+i1]` |
| `%3F6h` | (2, n) -> x1 | `ldr x1, [sp+i2]` |
| `%4F6h` | (n, 2) | `str x1, [sp+i1]` |
| `%5F6h` | (1, 2) | `mov x0, x1` |
| `%6F6h` | (2, 1) | `mov x1, x0` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0F6h`
```asm
; generic
ld       r16, __arg16_2(r1)
std      r16, __arg16_1(r1)
```

### Variants
| Prefix | (i1, i2) | Body |
|---|---|---|
| `%0F6h` | generic | `ld/std` |
| `%1F6h` | (1, n) -> r3 | `ld r3, [sp+i2]` |
| `%2F6h` | (n, 1) | `std r3, [sp+i1]` |
| `%3F6h` | (2, n) -> r4 | `ld r4, [sp+i2]` |
| `%4F6h` | (n, 2) | `std r4, [sp+i1]` |
| `%5F6h` | (1, 2) | `mr r3, r4` |
| `%6F6h` | (2, 1) | `mr r4, r3` |

## Notes
- Pure stack-to-stack slot copy; `i1` and `i2` are both `sp:` displacements.
- Shadow register slots (`sp[0]`, `sp[1]`) collapse into direct register-to-register moves (e.g. `mov r10, r11` on amd64).
- amd64 covers both directions plus the cross-shadow pair; aarch64/ppc64le mirror the layout with `x0/x1` and `r3/r4` respectively.
- No GC barrier -- values are copied as opaque ptr-sized words.
