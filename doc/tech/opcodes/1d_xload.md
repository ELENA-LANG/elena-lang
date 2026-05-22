# 0x1D -- xload

- **Category:** SingleOp
- **Enum:** `ByteCode::XLoad`
- **Operand(s):** (none)
- **Reads:** `acc`, `index`, `[acc + index]` (4 bytes)
- **Writes:** `index`

## Semantics
`index := acc[index]` -- uses `index` as a byte offset into `acc`, loads a dword from there into `index`. Indexing is byte-scaled (callers pre-multiply by element size). 64-bit form is `0x1E xlload`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %1Dh`

```asm
  lea  rax, [rbx+rdx]
  mov  edx, dword ptr [rax]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %1Dh`

```asm
  lea  eax, [ebx+edx]
  mov  edx, dword ptr [eax]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %1Dh`

```asm
  add     x12, x10, __arg12_1
  ldrsw   x9, [x12]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %1Dh`

```asm
  add     r18, r18, r15
  lwz     r14, 0(r18)
```

## Notes
- Indexed field load: `index := *(acc + index)` as a 32-bit value. The offset in `index` is BYTE-scaled -- callers must pre-multiply by element size.
- amd64 uses `mov edx, [rax]` (32-bit dest zero-extends to 64); aarch64 uses sign-extending `ldrsw`; ppc64le uses zero-extending `lwz`. Upper 32 bits of `index` are therefore arch-specific.
- 64-bit field load counterpart is `xlload` (0x1E). For an immediate-offset variant use the `xfield` family.
- Preserves `acc`, `sp[0]`, `sp[1]`. Clobbers `index` and a scratch (`rax`/`eax`/`x12`/`r18`).
