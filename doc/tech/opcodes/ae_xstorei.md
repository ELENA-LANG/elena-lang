# 0xAE -- xstore i

- **Category:** DoubleOp
- **Enum:** `ByteCode::XStoreI`
- **Operand(s):** `xstore i:i`
- **Reads:** `acc`, `[acc + i]`
- **Writes:** `sp[0]` (register cache only -- no `sp` change)
- **Side effects:** None.

## Semantics
`sp[0] := acc[i]` -- read field `i` of the object referenced by `acc` and place the value into the cached top-of-stack slot. The "store" mnemonic refers to the abstract destination (`sp[0]`); the direction of the field access is a *load* from `acc[i]`. Compare with `geti` (0xA5), which loads the same field into `acc` instead.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline % 0AEh`

```asm
  mov  r10, [rbx + __arg32_1]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline % 0AEh`

```asm
  mov  esi, [ebx + __arg32_1]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline % 0AEh`

```asm
  add     x11, x10, __arg12_1
  ldr     x0, [x11]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline % 0AEh`

```asm
  addi    r16, r15, __arg16_1
  ld      r3, 0(r16)
```

## Notes

- Despite the "store" mnemonic, the *data movement* is a **load** from `acc[i]`; the "store" refers to the abstract destination (`sp[0]`).
- Writes only the register cache for `sp[0]`; physical `sp[0]` memory is *not* updated until the next `xflush sp:0` (0xA4).
- Compare with `geti` (0xA5) which loads the same field into `acc`.
- No bounds check; nil-`acc` will fault or read garbage.
- Cached destination register: amd64 `r10`, x32 `esi`, aarch64 `x0`, ppc64le `r3`.
