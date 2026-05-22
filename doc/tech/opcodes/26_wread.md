# 0x26 -- wread

- **Category:** SingleOp
- **Enum:** `ByteCode::WRead`
- **Operand(s):** (none)
- **Reads:** sp[0], index, acc
- **Writes:** memory at `[acc]`

## Semantics
Reads a word (2 bytes) from `sp[0][index*2]` and stores it as an integer at `[acc]`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %26h`

```asm
  mov  rsi, r10
  xor  rax, rax
  mov  ax, word ptr [rsi+rdx*2]
  mov  [rbx], rax
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %26h`

```asm
  xor  eax, eax
  mov  ax, word ptr [esi+edx*2]
  mov  dword ptr [ebx], eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %26h`

```asm
  lsl     x18, x9, #1
  add     x18, x0, x18
  ldrsw   x17, [x18]
  str     x17, [x10]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %26h`

```asm
  sldi    r19, r14, 1
  add     r19, r3, r19
  lwz     r17, 0(r19)
  stw     r17, 0(r15)
```

## Notes
- 16-bit element read with implicit x2 index scaling (`sp[0] + index*2`).
- Counterpart to `bread` (0x23, x1); the wider `read n` (0x95) handles x4/x8 strides.
- amd64/x32 zero-extend the high bits; aarch64 emits `ldrsw` which is sign-extending -- caller must be aware if reading unsigned UTF-16 code units.
- ppc64le's `lwz` here actually loads a **word (4 bytes)**, not a halfword -- verify in the .asm template if exact 2-byte semantics matter on POWER.
- No bounds check; caller-side responsibility.
