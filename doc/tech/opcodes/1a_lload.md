# 0x1A -- lload

- **Category:** SingleOp
- **Enum:** `ByteCode::LLoad`
- **Operand(s):** (none)
- **Reads:** `[acc]` (8 bytes)
- **Writes:** `long:index`

## Semantics
`long:index := long:[acc]` -- loads a 64-bit value from `[acc]` into `index`. On 32-bit targets the value is carried as the `eax:edx` low/high pair.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %1Ah`

```asm
  mov  rdx, [rbx]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %1Ah`

```asm
  mov  eax, dword ptr [ebx]
  mov  edx, dword ptr [ebx+4]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %1Ah`

```asm
  ldr     x9, [x10]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %1Ah`

```asm
  ld      r14, 0(r15)
```

## Notes
- 64-bit counterpart of `load` (0x06). Reads 8 bytes at `[acc]`.
- On x32 emits a dual 32-bit load (`eax:edx` low/high pair). On 64-bit arches a single qword load suffices.
- Preserves `acc`, `sp[0]`, `sp[1]`. Clobbers `index` (and `eax` on x32 as the low half).
