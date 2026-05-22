# 0x81 -- setdp

- **Category:** DoubleOp
- **Enum:** `ByteCode::SetDP`
- **Operand(s):** `set dp:i`
- **Reads:** `fp`
- **Writes:** `acc`

## Semantics
`acc := &dp[i]` -- load the address of a slot in the current data frame into `acc`. The argument is the signed frame displacement; positive addresses the current frame, negative addresses the previous one. To dereference the slot, use `load dp:disp` (0x8A) instead.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %81h`

```asm
  lea  rbx, [rbp + __arg32_1]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %81h`

```asm
  lea  ebx, [ebp + __arg32_1]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %81h`

```asm
  add     x10, x29, __arg12_1
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %81h`

```asm
  addi    r15, r31, __arg16_1
```

## Notes
- This is an `lea` (address-of), not a value load -- use `load dp` (0x8A) to dereference.
- Argument is the abstract dp index; JIT rewrites it to a frame-relative byte offset before emit.
- Negative offsets address the previous frame, positive ones address the current frame.
- Preserves `index` and the cached `sp[0..1]`.
