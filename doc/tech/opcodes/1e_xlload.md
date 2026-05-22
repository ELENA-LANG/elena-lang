# 0x1E -- xlload

- **Category:** SingleOp
- **Enum:** `ByteCode::XLLoad`
- **Operand(s):** (none)
- **Reads:** `acc`, `index`, `[acc + index]` (8 bytes)
- **Writes:** `long:index`

## Semantics
`long:index := long:acc[index]` -- uses `index` as a byte offset into `acc`, loads a qword from there into `index`. Byte-scaled offset, same convention as `0x1D xload`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %1Eh`

```asm
  lea  rax, [rbx+rdx]
  mov  rdx, qword ptr [rax]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %1Eh`

```asm
  lea  eax, [ebx+edx]
  mov  edx, dword ptr [eax+4]
  mov  eax, dword ptr [eax]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %1Eh`

```asm
  add     x12, x10, __arg12_1
  ldr     x9, [x12]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %1Eh`

```asm
  add     r18, r18, r15
  ld      r14, 0(r18)
```

## Notes
- 64-bit counterpart of `xload` (0x1D). Reads a qword at `[acc + index]`. Byte-scaled offset -- callers pre-multiply.
- On x32 emits a dual 32-bit load: low half into `eax`, high half into `edx`.
- Preserves `acc`, `sp[0]`, `sp[1]`. Clobbers `index` and a scratch.
