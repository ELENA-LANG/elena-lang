# 0x06 -- load

- **Category:** SingleOp
- **Enum:** `ByteCode::Load`
- **Operand(s):** (none)
- **Reads:** `[acc]` (first 32-bit cell of the object pointed to by `acc`)
- **Writes:** `index`

## Semantics
Loads a 32-bit signed value from `[acc]` (offset 0) into `index`. On 64-bit targets the value is sign-extended to the full register width.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %6`

```asm
  movsxd  rdx, dword ptr [rbx]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %6`

```asm
  mov  edx, dword ptr [ebx]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %6`

```asm
  ldrsw   x9, [x10]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %6`

```asm
  lwz     r14, 0(r15)
```

## Notes
- 32-bit load from `[acc]`; 64-bit counterpart is `lload` (0x1A).
- Sign-extension is NOT uniform across arches: amd64 uses `movsxd` (sign-extend), aarch64 uses `ldrsw` (sign-extend), but ppc64le uses `lwz` (zero-extend) and x32 simply writes `edx`. Callers must not assume the upper 32 bits.
- Preserves `acc`, `sp[0]`, `sp[1]`. Clobbers `index` only.
