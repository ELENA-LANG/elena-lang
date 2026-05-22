# 0x2A -- xpeekeq

- **Category:** SingleOp
- **Enum:** `ByteCode::XPeekEq`
- **Operand(s):** (none)
- **Reads:** CPU EQ flag, sp[0], acc
- **Writes:** acc

## Semantics
Conditional move: if the previously-set CMP/EQ flag is true, `acc := sp[0]`; otherwise `acc` is unchanged. No stack adjustment.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %2Ah`

```asm
  cmovz rbx, r10
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %2Ah`

```asm
  cmovz ebx, esi
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %2Ah`

```asm
  csel    x10, x0, x10, eq
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %2Ah`

```asm
  iseleq  r15, r3, r15
```

## Notes
- Conditional-move primitive: `acc := COMP.EQ ? sp[0] : acc`. Used to fuse compare + select into a single branchless step.
- Reads the EQ flag set by a prior compare opcode (e.g. `xcmp`, `xlcmp`); requires those flags to be live.
- Does not pop the stack -- `sp[0]` remains for any follow-on opcode.
- `index` and stack contents are preserved; only `acc` may change.
- aarch64 `csel` and ppc64le `iseleq` are the canonical branchless-select forms, mirroring the x86 `cmovz`.
