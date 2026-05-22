# 0x22 -- neg

- **Category:** SingleOp
- **Enum:** `ByteCode::Neg`
- **Operand(s):** (none)
- **Reads:** index
- **Writes:** index

## Semantics
Two's-complement negation of `index`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %22h`

```asm
  neg    rdx
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %22h`

```asm
  neg    edx
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %22h`

```asm
  mov    x17, 0
  sub    x9, x17, x9
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %22h`

```asm
  neg    r14, r14
```

## Notes
- 32-bit (effectively whole-register on amd64/ppc64le) arithmetic negation of `index`.
- amd64/x32 `neg` updates ZF/SF/CF/OF as a side-effect; do not rely on those flags being meaningful afterward.
- aarch64 emits `mov xN, 0; sub xN, 0, x9` because the simpler `neg` alias is reserved by `mvn`/`sub` conventions in this template.
- Negating `INT_MIN` overflows silently -- same behavior across all four backends.
- `acc` and stack are preserved.
