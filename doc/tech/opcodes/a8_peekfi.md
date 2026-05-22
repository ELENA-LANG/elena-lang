# 0xA8 -- peek fp

- **Category:** DoubleOp
- **Enum:** `ByteCode::PeekFI`
- **Operand(s):** `peek fp:i`
- **Reads:** `fp[i]`
- **Writes:** `acc`
- **Side effects:** None.

## Semantics
`acc := fp[i]` -- pointer-sized load from the current data frame into `acc`. Inverse of `store fp:i` (0xA1).

## JIT (amd64)

**Selection rule:** dispatched through `loadFrameIndexOp` (`jitcompiler.cpp:980`) -- `retrieveCodeWithNegative(scope)` (`jitcompiler.cpp:634`) picks the inline variant by the *sign* of `i` so the immediate stays positive for narrow encodings, and the operand slot is rewritten as `getFPOffset(i << indexPower, frameOffset)`.

**Template:** `asm/amd64/core60.asm` `inline %0A8h`

```asm
  mov  rbx, qword ptr [rbp + __arg32_1]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0A8h`

```asm
  mov  ebx, [ebp + __arg32_1]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0A8h`

```asm
  sub     x11, x29, -__arg12_1
  ldr     x10, [x11]
```

### Variants
| Prefix | When | Body |
|---|---|---|
| `%0A8h` | `i < 0` (encoded `-i`) | `sub x11, x29, -i; ldr x10, [x11]` |
| `%5A8h` | `i > 0`                | `add x11, x29, i; ldr x10, [x11]` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0A8h`

```asm
  ld       r15, __arg16_1(r31)
```

## Notes

- Pointer-sized load from the data frame into `acc`. Positive `i` is current-frame, negative `i` indexes the caller's argument area through the saved-FP chain.
- Inverse of `store fp:i` (0xA1).
- Preserves `index` and the stack cache.
- aarch64 template is split by sign of `i` so the 12-bit immediate fits.
