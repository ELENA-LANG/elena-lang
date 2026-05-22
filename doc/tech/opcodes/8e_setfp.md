# 0x8E -- set fp

- **Category:** DoubleOp
- **Enum:** `ByteCode::SetFP`
- **Operand(s):** `set fp:i`
- **Reads:** `fp`
- **Writes:** `acc`

## Semantics
`acc := &fp[i]` -- load the address of frame slot `i` into the accumulator. Despite the name, this is an `lea`, not a value load. On amd64/x32 `fp` and `dp` are both `rbp`/`ebp`-relative, so the shape matches `setdp` (0x81); on aarch64 the sign of `i` selects a different variant slot (which the JIT inverts before emitting).

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %8Eh`

**Selection rule:** `retrieveCode` selects the offset-encoding variant; the argument is rewritten as `getFPOffset(arg1, dataOffset)` so the emitted displacement is the actual frame-relative byte offset. On aarch64 the sign of `i` picks between the `sub` (negative) and `add` (positive) variant slots.

```asm
  lea  rbx, qword ptr [rbp + __arg32_1]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %08Eh`

```asm
  lea  ebx, [ebp + __arg32_1]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %08Eh`

```asm
  sub     x10, x29, -__arg12_1
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%08Eh` | `i < 0` (inverted by JIT) | `sub x10, x29, -__arg12_1` |
| `%58Eh` | `i > 0` | `add x10, x29, __arg12_1` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %08Eh`

```asm
  li      r16, __arg16_1
  add     r15, r31, r16
```

## Notes
- Address-of, not a value load -- use `load dp` (0x8A) to dereference.
- On amd64/x32 `fp` and `dp` are both rbp-relative, so the body shape matches `setdp` (0x81).
- aarch64 splits the variant by sign of `i` (the JIT inverts negative offsets before emit).
- Argument is the abstract dp index; JIT rewrites it via `getFPOffset`.
