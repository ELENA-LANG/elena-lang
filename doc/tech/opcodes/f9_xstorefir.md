# 0xF9 -- xstore fp

- **Category:** TripleOp
- **Enum:** `ByteCode::XStoreFIR`
- **Operand(s):** `xstore fp:i, r` (R-cmd)

## Semantics
`fp[i] := r` -- store an immediate reference into a frame slot.

## JIT (amd64)
**Selection rule:** R-cmd -- variant by the `mskXxx` class of the stored ref combined with the sign of `arg1`. The displacement is rewritten as `getFPOffset(arg)` so positive vs. negative offsets pick different slots on aarch64 (add vs. sub addressing). Sentinel `r == 0` gets a dedicated zero-store slot.

**Template:** `asm/amd64/core60.asm` `inline %0F9h`
```asm
mov  rax, __ptr64_2
mov  [rbp+__arg32_1], rax
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0F9h`
```asm
mov  eax, __ptr32_2
mov  [ebp+__arg32_1], eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0F9h`
```asm
; arg1 < 0 (inverted by jitcompiler)
movz    x11, __ptr32lo_2
movk    x11, __ptr32hi_2, lsl #16
sub     x12, x29, -__arg12_1
str     x11, [x12]
```

### Variants
| Prefix | Form |
|---|---|
| `%0F9h` | arg1 < 0, r reference |
| `%4F9h` | arg1 > 0, r reference |
| `%5F9h` | arg1 < 0, r = 0 |
| `%9F9h` | arg1 > 0, r = 0 |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0F9h`
```asm
ld      r16, toc_code(r2)
addis   r16, r16, __xdisp32hi_2
addi    r16, r16, __xdisp32lo_2
std     r16, __arg16_1(r31)
```

### Variants
| Prefix | Form |
|---|---|
| `%0F9h` | r reference |
| `%5F9h` | r = 0 (`li r16, 0; std r16, [r31+i]`) |

## Notes
- Stores the immediate ref `r` into the frame slot `fp[i]`.
- aarch64 inverts the offset sign in the JIT -- the variant prefix encodes whether to emit `add x12, x29, +disp` or `sub x12, x29, -disp`.
- Sentinel `r == 0` skips the ptr-materialisation step and stores a zero directly.
- `arg` is rewritten as `getFPOffset(arg)` before encoding.
