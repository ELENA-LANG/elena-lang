# 0xA1 -- store fp

- **Category:** DoubleOp
- **Enum:** `ByteCode::StoreFI`
- **Operand(s):** `store fp:i`
- **Reads:** `acc`, `fp`
- **Writes:** `fp[i]` (pointer-sized slot)
- **Side effects:** None.

## Semantics
`fp[i] := acc` -- spill the managed `acc` register into the local-frame slot at offset `i`. No write barrier is required: the GC walks the frame from the frame header, so frame slots are already roots.

## JIT (amd64)

**Selection rule:** dispatched through `loadFrameIndexOp` (`jitcompiler.cpp:980`) -- `retrieveCode(arg1)` selects the inline template and the operand slot is rewritten as `getFPOffset(arg1 << indexPower, frameOffset)`. The aarch64 sign-based variant split (positive vs negative `i`) is driven by `retrieveCodeWithNegative` (`jitcompiler.cpp:634`) so the ARM 12-bit immediate stays unsigned.

**Template:** `asm/amd64/core60.asm` `inline %0A1h`

```asm
  mov  qword ptr [rbp + __arg32_1], rbx
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0A1h`

```asm
  mov  [ebp + __arg32_1], ebx
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0A1h`

```asm
  sub     x11, x29, -__arg12_1
  str     x10, [x11]
```

### Variants
| Prefix | When | Body |
|---|---|---|
| `%0A1h`  | `i < 0` (encoded as `-arg`) | `sub x11, x29, -arg; str x10, [x11]` |
| `%05A1h` | `i > 0`                     | `add x11, x29, arg; str x10, [x11]` |

The JIT picks the variant by sign of `arg1` so the immediate stays a 12-bit positive.

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0A1h`

```asm
  std     r15, __arg16_1(r31)
```

`std` stores 8 bytes; `r15 = acc`, `r31 = fp`.

## Notes

- Stores `acc` to `fp[i]`: positive `i` indexes the current frame, negative `i` indexes the caller's argument slots.
- Preserves `index`, `sp[0..1]` cache, and the GC card table -- frame slots are GC roots so no write barrier is needed.
- Inverse of `peek fp:i` (0xA8).
- aarch64 splits the template on `sign(i)` to keep the `add`/`sub` immediate within `imm12`.
