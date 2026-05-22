# 0x23 -- bread

- **Category:** SingleOp
- **Enum:** `ByteCode::BRead`
- **Operand(s):** (none)
- **Reads:** sp[0], index, acc
- **Writes:** memory at `[acc]`

## Semantics
Reads a zero-extended byte from `sp[0][index]` and stores it as an integer at `[acc]`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %23h`

```asm
  mov  rsi, r10
  xor  rax, rax
  mov  al, byte ptr [rsi+rdx]
  mov  [rbx], rax
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %23h`

```asm
  xor  eax, eax
  mov  al, byte ptr [esi+edx]
  mov  dword ptr [ebx], eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %23h`

```asm
  add     x18, x0, x9
  ldrsb   x17, [x18]
  str     x17, [x10]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %23h`

```asm
  add     r19, r3, r14
  lbz     r17, 0(r19)
  std     r17, 0(r15)
```

## Notes
- Byte-element read with implicit x1 index scaling -- `index` is a raw byte offset.
- For wider element reads use `wread` (0x26, x2) or `read n` (0x95, parametrized x4/x8).
- amd64 uses zero-extending `mov al, ...` after `xor rax, rax`; aarch64 uses **sign-extending** `ldrsb` while ppc64le uses zero-extending `lbz` -- verify this mismatch if signed bytes are meaningful.
- Caller must ensure `sp[0] + index` is within the buffer; no bounds check is emitted.
- `acc` itself is read for the destination pointer but its value is not modified.
