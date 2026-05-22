# 0x79 -- fsqrt dp

- **Category:** DoubleOp
- **Enum:** `ByteCode::FSqrtDP`
- **Operand(s):** `disp` (frame-pointer displacement)
- **Reads:** `sp[0]` (pointer to double)
- **Writes:** `double:dp[disp]`

## Semantics
Computes the square root of the double pointed to by `sp[0]` and stores it at `dp[disp]`: `double:dp[disp] := sqrt(double:[sp[0]])`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %79h`

```asm
  mov   rax, r10
  lea   rdi, [rbp + __arg32_1]
  fld   qword ptr [rax]
  fsqrt
  fstp  qword ptr [rdi]    ; store result
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %79h`

```asm
  lea   edi, [ebp + __arg32_1]
  fld   qword ptr [esi]
  fsqrt
  fstp  qword ptr [edi]    ; store result
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %79h`

```asm
  add     x19, x29, __arg12_1
  ldr     d18, [x0]
  fsqrt   d18, d18
  str     d18, [x19]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %79h`

```asm
  addi    r19, r31, __arg16_1

  lfd     f17, 0(r3)
  fsqrt   f17, f17
  stfd    f17, 0(r19)
```

## Notes
- Hardware square root -- every supported backend has a single dedicated instruction (`fsqrt` everywhere).
- Negative inputs produce NaN per IEEE-754; no exception is raised unless the host has enabled invalid-op traps.
- Same load-op-store shape as `fabs` (0x78): load via `sp[0]`, compute, store at `dp[disp]`.
- Result precision is full double (53-bit mantissa); no reduced-precision shortcut path.
- `acc`, `index`, and stack pointer are preserved.
