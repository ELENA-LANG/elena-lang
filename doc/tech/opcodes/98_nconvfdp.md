# 0x98 -- nconf dp

- **Category:** DoubleOp
- **Enum:** `ByteCode::NConvFDP`
- **Operand(s):** `nconvf dp:disp`
- **Reads:** `acc` (pointer to source double), `fp`
- **Writes:** `dp[disp]` (32-bit int)
- **Side effects:** Uses the current FPU rounding mode on x86; aarch64/ppc64le use the chip's round-to-nearest / truncate instructions.

## Semantics
`double(acc) >> int dp[disp]` -- load the qword at `[acc]` as a double, convert to integer, store at `dp[disp]`. For round-toward-zero regardless of mode see `ftrunc dp:disp` (0x99).

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %098h`

```asm
  lea   rdi, [rbp + __arg32_1]
  fld   qword ptr [rbx]
  fistp dword ptr [rdi]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %098h`

```asm
  lea   edi, [ebp + __arg32_1]
  fld   qword ptr [ebx]
  fistp dword ptr [edi]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %098h`

```asm
  add      x19, x29, __arg12_1
  ldr      d0, [x10]
  frintx   d0, d0
  fcvtzs   x18, d0
  str      x18, [x19]
```

Round-to-nearest via `frintx`, then truncating int conversion via `fcvtzs`.

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %098h`

```asm
  addi    r19, r31, __arg16_1

  lfd     f17, 0(r15)
  fctidz  f18, f17
  stfd    f18, 0(r19)
```

`fctidz` performs the float->int conversion with round-toward-zero.

## Notes
- Source double loaded from `[acc]`; destination is a 32-bit int at `dp[disp]`.
- x86 uses the ambient FPU rounding mode (typically round-to-nearest); aarch64 explicitly `frintx` + `fcvtzs`; ppc64le uses `fctidz` (truncate).
- For explicit round-toward-zero on the source value see `ftruncdp` (0x99); for explicit round-to-nearest see `frounddp` (0x9F).
- No exception filtering -- invalid conversions silently produce the architecture's "indefinite" value.
