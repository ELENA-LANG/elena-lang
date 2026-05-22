# 0x0F -- wload

- **Category:** SingleOp
- **Enum:** `ByteCode::WLoad`
- **Operand(s):** (none)
- **Reads:** `[acc]` (16-bit cell)
- **Writes:** `index`

## Semantics
Loads a 16-bit signed word from `[acc]` and sign-extends it into `index`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %0Fh`

```asm
  mov  eax, dword ptr [ebx]
  cwde
  mov  edx, eax
```

`cwde` sign-extends `ax` into `eax`.

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0Fh`

```asm
  mov  eax, dword ptr [ebx]
  cwde
  mov  edx, eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0Fh`

```asm
  ldrsw   x9, [x10]
  sxth    x9, x9
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0Fh`

```asm
  lha     r14, 0(r15)
```

`lha` loads a halfword and sign-extends in a single instruction.

## Notes
- Sign-extends the low 16 bits at `[acc]` into `index`. Counterpart of `bload` (0x0E, zero-extend byte).
- amd64/x32 read a dword then sign-extend via `cwde`; aarch64 currently does `ldrsw` + `sxth` (double extension); ppc64le uses the single-instruction `lha`.
- Preserves `acc`, `sp[0]`, `sp[1]`. Clobbers `index` (and `eax` as a scratch on x86).
