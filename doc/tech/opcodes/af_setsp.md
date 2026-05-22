# 0xAF -- set sp

- **Category:** DoubleOp
- **Enum:** `ByteCode::SetSP`
- **Operand(s):** `set sp:i`
- **Reads:** `sp`
- **Writes:** `acc`
- **Side effects:** None.

## Semantics
`acc := &sp[i]` -- load the **address** of stack slot `i` into `acc` (effective-address form). Distinct from `peek sp:i` (0xA9) which loads the *value*. Typically used before `xnewn`/`xcreate` to build a stack-resident object header in front of the slot, or to take a managed reference into the frame.

## JIT (amd64)

**Selection rule:** `retrieveCode(arg)` selects the inline template; the operand is rewritten as `(scope->stackOffset + arg) << indexPower` so the `lea` reads the physical SP-relative byte offset, not the logical bytecode index.

**Template:** `asm/amd64/core60.asm` `inline % 0AFh`

```asm
  lea   rbx, [rsp + __arg32_1]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline % 0AFh`

```asm
  lea   ebx, [esp + __arg32_1]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline % 0AFh`

```asm
  add     x10, sp, __arg12_1
```

`add` plays the role of `lea`.

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline % 0AFh`

```asm
  li      r16, __arg16_1
  add     r15, r1, r16
```

## Notes

- Computes `acc := &sp[i]` (effective-address); does not touch memory.
- Typical use: building a stack-resident object header (`xnewn`/`xcreate`) or taking a managed reference into the frame to pass to a native helper.
- Distinct from `peek sp:i` (0xA9) which loads the *value*.
- aarch64 uses `add` to play the role of `lea`; ppc64le materialises the displacement into `r16` then adds.
