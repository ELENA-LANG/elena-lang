# 0x78 -- fabs dp

- **Category:** DoubleOp
- **Enum:** `ByteCode::FAbsDP`
- **Operand(s):** `disp` (frame-pointer displacement)
- **Reads:** `sp[0]` (pointer to double)
- **Writes:** `double:dp[disp]`

## Semantics
Computes the absolute value of the double pointed to by `sp[0]` and stores it at `dp[disp]`: `double:dp[disp] := abs(double:[sp[0]])`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %78h`

```asm
  mov   rax, r10
  lea   rdi, [rbp + __arg32_1]
  fld   qword ptr [rax]
  fabs
  fstp  qword ptr [rdi]    ; store result
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %78h`

```asm
  lea   edi, [ebp + __arg32_1]
  fld   qword ptr [esi]
  fabs
  fstp  qword ptr [edi]    ; store result
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %78h`

```asm
  add     x19, x29, __arg12_1
  ldr     d18, [x0]
  fabs    d18, d18
  str     d18, [x19]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %78h`

```asm
  addi    r19, r31, __arg16_1

  lfd     f17, 0(r3)
  fabs    f17, f17
  stfd    f17, 0(r19)
```

## Notes
- Reads a double via `sp[0]`, computes |x|, and stores the result at the frame-pointer-relative slot `dp[disp]`.
- amd64/x32 use the x87 `fabs` instruction (clears the sign bit on the top-of-stack); aarch64 and ppc64le have dedicated `fabs` mnemonics.
- A pure SSE form would be `andpd xmm, [sign_mask]`; this template chose x87 for compatibility with the rest of the FP pipeline.
- NaN propagates unchanged (sign bit cleared); +/-0 normalize to +0.
- `acc`, `index`, and stack pointer are preserved.
