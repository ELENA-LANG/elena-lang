# 0x7B -- fln dp

- **Category:** DoubleOp
- **Enum:** `ByteCode::FLnDP`
- **Operand(s):** `disp` (frame-pointer displacement)
- **Reads:** `sp[0]` (pointer to double)
- **Writes:** `double:dp[disp]`
- **Side effects:** clobbers FPU stack (x87 backends), reads `CORE_MATH_TABLE` (RISC backends)

## Semantics
Computes the natural logarithm of the double pointed to by `sp[0]`, storing the result at `dp[disp]`: `double:dp[disp] := ln(double:[sp[0]])`. x87 backends use the `fyl2x` instruction with `ln(2)` as the multiplier; RISC backends extract the exponent/mantissa manually and evaluate an inline polynomial approximation using the `fm_log2_p` / `fm_log2_q` constant tables stored at `CORE_MATH_TABLE`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %7Bh`

```asm
  mov   rax, r10
  lea   rdi, [rbp + __arg32_1]
  fld   qword ptr [rax]

  fldln2
  fxch
  fyl2x                   ; ->[log2(Src)]*ln(2) = ln(Src)

  fstsw ax                ; retrieve exception flags from FPU
  shr   al,1              ; test for invalid operation
  jc    short lErr        ; clean-up and return error

  fstp  qword ptr [rdi]    ; store result
  mov   edx, 1
  jmp   short labEnd

lErr:
  ffree st(0)

labEnd:
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %7Bh`

```asm
  lea   edi, [ebp + __arg32_1]
  fld   qword ptr [esi]

  fldln2
  fxch
  fyl2x                   ; ->[log2(Src)]*ln(2) = ln(Src)

  fstsw ax                ; retrieve exception flags from FPU
  shr   al,1              ; test for invalid operation
  jc    short lErr        ; clean-up and return error

  fstp  qword ptr [edi]    ; store result
  mov   edx, 1
  jmp   short labEnd

lErr:
  ffree st(0)

labEnd:
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %7Bh`

```asm
#if _MAC
  adrp    x17, rdata_page : %CORE_MATH_TABLE
  add     x17, x17, rdata_pageoff : %CORE_MATH_TABLE
#elif (_LNX || _FREEBSD)
  movz    x17, rdata_ptr32lo : %CORE_MATH_TABLE
  movk    x17, rdata_ptr32hi : %CORE_MATH_TABLE, lsl #16
#endif

  add     x19, x29, __arg12_1 ; dest (x19)

  ldr     d17, [x0]           ; x (d17)

  ;   val.f = x;
  str     d17, [x19]

  ; extract exponent and part of the mantissa

  ;   fpart = val.s.i1 & FM_DOUBLE_MMASK;
  movz    x20, #0FFFFh
  movk    x20, #0Fh, lsl #16

  ldrsw   x16, [x19, #4]
  and     x16, x16, x20
  ;   ipart = val.s.i1 & FM_DOUBLE_EMASK;
  movz    x20, #0
  movk    x20, #7FF0h, lsl #16
  ldrsw   x18, [x19, #4]
  and     x18, x18, x20

  ;   /* set exponent to 0 to get the prefactor to 2**ipart */
  ;   fpart |= FM_DOUBLE_EZERO;
  movz    x20, #0
  movk    x20, #3FF0h, lsl #16
  orr     x16, x16, x20
  ;   val.s.i1 = fpart;
  str     w16, [x19, #4]
  ;   x = val.f;
  ldr     d17, [x19]

  ;   /* extract exponent */
  ;   ipart >>= FM_DOUBLE_MBITS;
  movz    x20, #20
  lsr     x18, x18, x20

  ;   ipart -= FM_DOUBLE_BIAS;
  movz    x20, #1023
  sub     x18, x18, x20

  ;   /* the polynomial is computed for sqrt(0.5) < x < sqrt(2),
  ;      but we have the mantissa in the interval 1 < x < 2.
  ;      adjust by dividing x by 2 and incrementing ipart, if needed. */
  ;   if (x > FM_DOUBLE_SQRT2) {
  ldr     d20, [x17, #64]
  fcmp    d17, d20
  blt     labSkip
  beq     labSkip

  ;      x *= 0.5;
  ldr     d20, [x17, #40]
  fmul    d17, d17, d20
  ;      ++ipart;
  add     x18, x18, #1
  ;   }
labSkip:

  ;   /* use polynomial approximation for log(1+x) */
  ;   x -= 1.0;
  ldr     d20, [x17, #32]
  fsub    d17, d17, d20

  ;   px = fm_log2_p[0];
  ldr     d18, [x17, #72]

  ;   px = px * x + fm_log2_p[1];
  ldr     d20, [x17, #80]
  fmul    d18, d18, d17
  fadd    d18, d18, d20

  ;   px = px * x + fm_log2_p[2];
  ldr     d20, [x17, #88]
  fmul    d18, d18, d17
  fadd    d18, d18, d20

  ;   px = px * x + fm_log2_p[3];
  ldr     d20, [x17, #96]
  fmul    d18, d18, d17
  fadd    d18, d18, d20

  ;   px = px * x + fm_log2_p[4];
  ldr     d20, [x17, #104]
  fmul    d18, d18, d17
  fadd    d18, d18, d20

  ;   px = px * x + fm_log2_p[5];
  ldr     d20, [x17, #112]
  fmul    d18, d18, d17
  fadd    d18, d18, d20

  ;   qx = x + fm_log2_q[0];
  ldr     d20, [x17, #120]
  fadd    d19, d17, d20

  ;   qx = qx * x + fm_log2_q[1];
  ldr     d20, [x17, #128]
  fmul    d19, d19, d17
  fadd    d19, d19, d20

  ;   qx = qx * x + fm_log2_q[2];
  ldr     d20, [x17, #136]
  fmul    d19, d19, d17
  fadd    d19, d19, d20

  ;   qx = qx * x + fm_log2_q[3];
  ldr     d20, [x17, #144]
  fmul    d19, d19, d17
  fadd    d19, d19, d20

  ;   qx = qx * x + fm_log2_q[4];
  ldr     d20, [x17, #152]
  fmul    d19, d19, d17
  fadd    d19, d19, d20

  ;   z = x * x;
  fmul    d21, d17, d17

  ;   z = x * (z * px / qx) - 0.5 * z + x;
  fmul    d20, d21, d18
  fdiv    d20, d20, d19
  fmul    d22, d20, d17

  ldr     d20, [x17, #40]
  fmul    d20, d20, d21
  fsub    d22, d22, d20
  fadd    d21, d22, d17

  ;   z += ((double)ipart) * FM_DOUBLE_LOGEOF2;
  scvtf   d20, x18
  ldr     d18, [x17, #160]
  fmul    d20, d20, d18
  fadd    d21, d21, d20

  str     d21, [x19]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %7Bh`

```asm
  ld      r17, toc_rdata(r2)
  addis   r17, r17, rdata_disp32hi : %CORE_MATH_TABLE
  addi    r17, r17, rdata_disp32lo : %CORE_MATH_TABLE

  addi    r19, r31, __arg16_1 ; dest (r19)

  lfd     f17, 0(r3)          ; x (f17)

  ;   val.f = x;
  stfd    f17, 0(r19)

  ; extract exponent and part of the mantissa

  ;   fpart = val.s.i1 & FM_DOUBLE_MMASK;
  li      r20, -1
  andi.   r20, r20, 0FFFFh
  addis   r20, r20, 0Fh
  lwz     r16, 4(r19)
  and     r16, r16, r20
  ;   ipart = val.s.i1 & FM_DOUBLE_EMASK;
  lis     r20, 7FF0h
  lwz     r18, 4(r19)
  and     r18, r18, r20

  ;   /* set exponent to 0 to get the prefactor to 2**ipart */
  ;   fpart |= FM_DOUBLE_EZERO;
  lis     r20, 3FF0h
  or      r16, r16, r20
  ;   val.s.i1 = fpart;
  stw     r16, 4(r19)
  ;   x = val.f;
  lfd     f17, 0(r19)

  ;   /* extract exponent */
  ;   ipart >>= FM_DOUBLE_MBITS;
  li      r20, 20
  srd     r18, r18, r20

  ;   ipart -= FM_DOUBLE_BIAS;
  li      r20, 1023
  sub     r18, r18, r20

  ;   /* the polynomial is computed for sqrt(0.5) < x < sqrt(2),
  ;      but we have the mantissa in the interval 1 < x < 2.
  ;      adjust by dividing x by 2 and incrementing ipart, if needed. */
  ;   if (x > FM_DOUBLE_SQRT2) {
  lfd     f20, 64(r17)
  fcmpu   f17, f20
  blt     labSkip
  beq     labSkip

  ;      x *= 0.5;
  lfd     f20, 40(r17)
  fmul    f17, f17, f20
  ;      ++ipart;
  addi    r18, r18, 1
  ;   }
labSkip:

  ;   /* use polynomial approximation for log(1+x) */
  ;   x -= 1.0;
  lfd     f20, 32(r17)
  fsub    f17, f17, f20

  ;   px = fm_log2_p[0];
  lfd     f18, 72(r17)

  ;   px = px * x + fm_log2_p[1];
  lfd     f20, 80(r17)
  fmul    f18, f18, f17
  fadd    f18, f18, f20

  ;   px = px * x + fm_log2_p[2];
  lfd     f20, 88(r17)
  fmul    f18, f18, f17
  fadd    f18, f18, f20

  ;   px = px * x + fm_log2_p[3];
  lfd     f20, 96(r17)
  fmul    f18, f18, f17
  fadd    f18, f18, f20

  ;   px = px * x + fm_log2_p[4];
  lfd     f20, 104(r17)
  fmul    f18, f18, f17
  fadd    f18, f18, f20

  ;   px = px * x + fm_log2_p[5];
  lfd     f20, 112(r17)
  fmul    f18, f18, f17
  fadd    f18, f18, f20

  ;   qx = x + fm_log2_q[0];
  lfd     f20, 120(r17)
  fadd    f19, f17, f20

  ;   qx = qx * x + fm_log2_q[1];
  lfd     f20, 128(r17)
  fmul    f19, f19, f17
  fadd    f19, f19, f20

  ;   qx = qx * x + fm_log2_q[2];
  lfd     f20, 136(r17)
  fmul    f19, f19, f17
  fadd    f19, f19, f20

  ;   qx = qx * x + fm_log2_q[3];
  lfd     f20, 144(r17)
  fmul    f19, f19, f17
  fadd    f19, f19, f20

  ;   qx = qx * x + fm_log2_q[4];
  lfd     f20, 152(r17)
  fmul    f19, f19, f17
  fadd    f19, f19, f20

  ;   z = x * x;
  fmul    f21, f17, f17

  ;   z = x * (z * px / qx) - 0.5 * z + x;
  fmul    f20, f21, f18
  fdiv    f20, f20, f19
  fmul    f22, f20, f17

  lfd     f20, 40(r17)
  fmul    f20, f20, f21
  fsub    f22, f22, f20
  fadd    f21, f22, f17

  ;   z += ((double)ipart) * FM_DOUBLE_LOGEOF2;
  std     r18, 0(r19)
  lfd     f20, 0(r19)
  fcfid   f20, f20
  lfd     f18, 160(r17)
  fmul    f20, f20, f18
  fadd    f21, f21, f20

  stfd    f21, 0(r19)
```

## Notes
- x87 backends emit `fldln2; fxch; fyl2x` (one instruction does `y*log2(x)`).
- aarch64/ppc64le split the work into exponent extraction + rational polynomial over `fm_log2_p`/`fm_log2_q` from `%CORE_MATH_TABLE`.
- Input must be > 0 -- negative or zero produces NaN/-Inf; x87 backends raise an FPU invalid-op flag, x86 paths jump to `lErr` and set `edx = 0`.
- The 6th/4th polynomial coefficients live at MATH_TABLE offsets 72..152 (aarch64 layout); ppc64le mirrors the same offsets.
- Constant slot at offset 160 (`FM_DOUBLE_LOGEOF2`) is used to translate log2 back to ln; reusing this constant from MATH_TABLE avoids inlining the literal.
