# 0xF3 -- mov sp

- **Category:** TripleOp
- **Enum:** `ByteCode::MovSIFI`
- **Operand(s):** `mov sp:i1, fp:i2`

## Semantics
`sp[i1] := fp[i2]` -- copy a frame slot into a stack slot.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %0F3h`
```asm
; generic
mov  rax, qword ptr [rbp+__arg32_2]
mov  qword ptr [rsp+__arg32_1], rax
```

### Variants
| Prefix | i1 | Body |
|---|---|---|
| `%0F3h` | generic | `mov [rsp+i1], rax` |
| `%1F3h` | 0 | `mov r10, [rbp+i2]` |
| `%2F3h` | 1 | `mov r11, [rbp+i2]` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0F3h`
```asm
; generic
mov  eax, [ebp+__arg32_2]
mov  [esp+__arg32_1], eax
```

### Variants
| Prefix | i1 | Body |
|---|---|---|
| `%0F3h` | generic | `mov [esp+i1], eax` |
| `%1F3h` | 0 | `mov esi, [ebp+i2]` |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0F3h`
```asm
; arg2 < 0 (%0F3h)
sub     x12, x29, -__arg12_2
add     x13, sp, __arg12_1

ldr     x11, [x12]
str     x11, [x13]
```

### Variants
| Prefix | i1, sign | Body |
|---|---|---|
| `%0F3h` | generic, fp-disp < 0 | `ldr/str` |
| `%1F3h` | sp:0, fp:i2<0 | `ldr x0, [x29-i2]` |
| `%2F3h` | sp:1, fp:i2<0 | `ldr x1, [x29-i2]` |
| `%5F3h` | generic, fp-disp > 0 | `add x12, x29, +disp` |
| `%6F3h` | sp:0, fp:i2>0 | `ldr x0, [x29+i2]` |
| `%7F3h` | sp:1, fp:i2>0 | `ldr x1, [x29+i2]` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0F3h`
```asm
ld       r16, __arg16_2(r31)
std      r16, __arg16_1(r1)
```

### Variants
| Prefix | i1 | Body |
|---|---|---|
| `%0F3h` | generic | `ld/std` |
| `%1F3h` | 0 | `ld r3, [r31+i2]` |
| `%2F3h` | 1 | `ld r4, [r31+i2]` |

## Notes
- Source is the frame slot `fp[i2]`; destination is the stack slot `sp[i1]` (or its shadow register if `i1  in  {0,1}`).
- aarch64 also splits on the sign of `i2` because the offset must be encoded with the matching `add`/`sub` instruction.
- Used to push a captured local/parameter onto the stack temp area before a call.
