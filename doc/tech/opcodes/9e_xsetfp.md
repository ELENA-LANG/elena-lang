# 0x9E -- xset fp

- **Category:** DoubleOp
- **Enum:** `ByteCode::XSetFP`
- **Operand(s):** `xset fp:i`
- **Reads:** `fp`, `index`
- **Writes:** `acc`

## Semantics
`acc := &fp[index + i]` -- load the address of a frame slot at dynamic offset `index` plus static displacement `i`. Index is scaled by the slot size (8 on 64-bit, 4 on 32-bit). Companion to `setfp` (0x8E) which has no dynamic index.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %09Eh`

**Selection rule:** `retrieveCode` selects the offset-encoding variant; the argument is rewritten as `getFPOffset(arg1, dataOffset)` so the emitted displacement is the actual frame-relative byte offset. On aarch64 the sign of `i` picks between the `sub` (negative) and `add` (positive) variant slots.

```asm
  lea  rax, [rdx*8]
  lea  rbx, [rbp + rax + __arg32_1]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %09Eh`

```asm
  lea  eax, [edx*4]
  lea  ebx, [ebp + eax + __arg32_1]
```

Scale factor is 4 (32-bit slot size).

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %09Eh`

```asm
  lsl     x14, x9, #3
  sub     x10, x29, -__arg12_1
  sub     x10, x10, x14
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%09Eh` | `i < 0` (inverted by JIT) | `sub x10, x29, -__arg12_1` then `sub` index |
| `%59Eh` | `i > 0` | `add x10, x29, __arg12_1` then `add` index |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %09Eh`

```asm
  li      r16, __arg16_1
  add     r15, r31, r16
  sldi    r18, r14, 3
  add     r15, r15, r18
```

## Notes
- Companion to `setfp` (0x8E) -- adds a dynamic `index`-scaled offset on top of the static displacement.
- Index is scaled by slot size (8 on 64-bit, 4 on x32).
- Address-of, not a value load.
- aarch64 splits variant by sign of `i` (the JIT inverts negative offsets before emit).
