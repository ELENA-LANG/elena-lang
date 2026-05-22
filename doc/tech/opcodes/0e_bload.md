# 0x0E -- bload

- **Category:** SingleOp
- **Enum:** `ByteCode::BLoad`
- **Operand(s):** (none)
- **Reads:** `[acc]` (byte 0)
- **Writes:** `index`

## Semantics
Zero-extends the low byte of `[acc]` into `index`. The amd64 / x32 form reads a full dword and masks with `0xFF`; aarch64 uses `ldrb` directly.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %0Eh`

```asm
  mov  edx, dword ptr [ebx]
  and  edx, 0FFh
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0Eh`

```asm
  mov  edx, dword ptr [ebx]
  and  edx, 0FFh
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0Eh`

```asm
  ldrb    w9, [x10]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0Eh`

```asm
  lwz     r14, 0(r15)
  andi.   r14, r14, 0FFh
```

## Notes
- Zero-extends the low byte of `[acc]` into `index`. Pairs conceptually with `wload` (0x0F) for halfword and `load` (0x06) for dword.
- amd64/x32 read a full dword then mask; aarch64 uses `ldrb` (single-instruction zero-extend); ppc64le loads with `lwz` and masks because there is no direct byte-zero-extending load that targets a GPR here.
- Preserves `acc`, `sp[0]`, `sp[1]`. Clobbers `index`.
