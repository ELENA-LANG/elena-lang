# 0x7A -- fexp dp

- **Category:** DoubleOp
- **Enum:** `ByteCode::FExpDP`
- **Operand(s):** `disp` (frame-pointer displacement)
- **Reads:** `sp[0]` (pointer to double)
- **Writes:** `double:dp[disp]`
- **Side effects:** clobbers FPU stack (x87 backends), reads `CORE_MATH_TABLE` (RISC backends)

## Semantics
Computes the exponential `e^x` where `x` is the double pointed to by `sp[0]`, storing the result at `dp[disp]`: `double:dp[disp] := exp(double:[sp[0]])`. x87 backends (amd64/x32) implement this with the `f2xm1`/`fscale` sequence on the FPU; RISC backends (aarch64/ppc64le) evaluate an inline polynomial approximation using the `fm_exp2_p` / `fm_exp2_q` constant tables stored at `CORE_MATH_TABLE`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %7Ah`

```asm
  mov   rax, r10
  lea   rdi, [rbp + __arg32_1]
  fld   qword ptr [rax]
  xor   edx, edx

  fldl2e                  ; ->log2(e)
  fmulp                   ; ->log2(e)*Src

  ; the FPU can compute the antilog only with the mantissa
  ; the characteristic of the logarithm must thus be removed

  fld   st(0)             ; copy the logarithm
  frndint                 ; keep only the characteristic
  fsub  st(1),st(0)       ; keeps only the mantissa
  fxch                    ; get the mantissa on top

  f2xm1                   ; ->2^(mantissa)-1
  fld1
  faddp                   ; add 1 back

  ; the number must now be readjusted for the characteristic of the logarithm

  fscale                  ; scale it with the characteristic

  fstsw ax                ; retrieve exception flags from FPU
  shr   al,1              ; test for invalid operation
  jc    short lErr        ; clean-up and return if error

  ; the characteristic is still on the FPU and must be removed

  fstp  st(1)             ; get rid of the characteristic

  fstp  qword ptr [rdi]    ; store result
  mov   edx, 1
  jmp   short labEnd

lErr:
  ffree st(1)

labEnd:
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %7Ah`

```asm
  lea   edi, [ebp + __arg32_1]
  fld   qword ptr [esi]
  xor   edx, edx

  fldl2e                  ; ->log2(e)
  fmulp                   ; ->log2(e)*Src

  ; the FPU can compute the antilog only with the mantissa
  ; the characteristic of the logarithm must thus be removed

  fld st(0)               ; copy the logarithm
  frndint                 ; keep only the characteristic
  fsub  st(1),st(0)       ; keeps only the mantissa
  fxch                    ; get the mantissa on top

  f2xm1                   ; ->2^(mantissa)-1
  fld1
  faddp                   ; add 1 back

  ; the number must now be readjusted for the characteristic of the logarithm

  fscale                  ; scale it with the characteristic

  fstsw ax                ; retrieve exception flags from FPU
  shr   al,1              ; test for invalid operation
  jc    short lErr        ; clean-up and return if error

  ; the characteristic is still on the FPU and must be removed

  fstp  st(1)             ; get rid of the characteristic

  fstp  qword ptr [edi]    ; store result
  mov   edx, 1
  jmp   short labEnd

lErr:
  ffree st(1)

labEnd:
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %7Ah`

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

  ; x = x * FM_DOUBLE_LOG2OFE
  ldr     d20, [x17]
  fmul    d17, d17, d20

  ; ipart = x + 0.5
  ldr     d20, [x17, #40]
  fadd    d18, d17, d20
                              ; ipart(d18)
  frintz  d18, d18            ; ipart = floor(ipart)

  fsub    d19, d17, d18       ; fpart = x - ipart; (d19)

  ; FM_DOUBLE_INIT_EXP(epart,ipart);
  frintx  d20, d18
  fcvtzs  x18, d20
  add     x18, x18, #1023
  mov     x20, #20
  lsl     x18, x18, x20
  mov     x20, #0
  str     x20, [x19]
  str     w18, [x19, #4]

  fmul    d17, d19, d19       ; x = fpart*fpart;

  ldr     d20, [x17, #8]      ; px =        fm_exp2_p[0];

  ; px = px*x + fm_exp2_p[1];
  fmul    d20, d20, d17
  ldr     d21, [x17, #16]
  fadd    d20, d20, d21

  ; qx =    x + fm_exp2_q[0];
  ldr     d22, [x17, #48]
  fadd    d22, d22, d17

  ; px = px*x + fm_exp2_p[2];
  fmul    d20, d20, d17
  ldr     d21, [x17, #24]

  fadd    d20, d20, d21

  ; qx = qx*x + fm_exp2_q[1];
  fmul    d22, d22, d17
  ldr     d21, [x17, #56]
  fadd    d22, d22, d21

  ; px = px * fpart;
  fmul    d20, d20, d19

  ; x = 1.0 + 2.0*(px/(qx-px))
  ldr     d16, [x17, #32]

  fmov    d17, d16

  fadd    d16, d16, d16

  fsub    d21, d22, d20
  fdiv    d21, d20, d21
  fmul    d16, d16, d21
  fadd    d17, d17, d16

  ; epart.f*x;
  ldr     d20, [x19]
  fmul    d20, d20, d17
  str     d20, [x19]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %7Ah`

```asm
  ld      r17, toc_rdata(r2)
  addis   r17, r17, rdata_disp32hi : %CORE_MATH_TABLE
  addi    r17, r17, rdata_disp32lo : %CORE_MATH_TABLE

  addi    r19, r31, __arg16_1 ; dest (r19)

  lfd     f17, 0(r3)          ; x (f17)

  ; x = x * FM_DOUBLE_LOG2OFE
  lfd     f20, 0(r17)
  fmul    f17, f17, f20

  ; ipart = x + 0.5
  lfd     f20, 40(r17)
  fadd    f18, f17, f20
                              ; ipart(f18)
  friz    f18, f18            ; ipart = floor(ipart)

  fsub    f19, f17, f18       ; fpart = x - ipart; (f19)

  ; FM_DOUBLE_INIT_EXP(epart,ipart);
  fctidz  f20, f18
  stfd    f20, 0(r19)
  ld      r18, 0(r19)
  li      r20, 1023
  add     r18, r18, r20
  li      r20, 20
  sld     r18, r18, r20
  li      r20, 0
  std     r20, 0(r19)
  stw     r18, 4(r19)

  fmul    f17, f19, f19       ; x = fpart*fpart;

  lfd     f20, 8(r17)         ; px =        fm_exp2_p[0];

  ; px = px*x + fm_exp2_p[1];
  fmul    f20, f20, f17
  lfd     f21, 16(r17)
  fadd    f20, f20, f21

  ; qx =    x + fm_exp2_q[0];
  lfd     f22, 48(r17)
  fadd    f22, f22, f17

  ; px = px*x + fm_exp2_p[2];
  fmul    f20, f20, f17
  lfd     f21, 24(r17)
  fadd    f20, f20, f21

  ; qx = qx*x + fm_exp2_q[1];
  fmul    f22, f22, f17
  lfd     f21, 56(r17)
  fadd    f22, f22, f21

  ; px = px * fpart;
  fmul    f20, f20, f19

  ; x = 1.0 + 2.0*(px/(qx-px))
  lfd     f16, 32(r17)

  ; mr      f17, f16
  stfd    f16, -8(r1)
  lfd     f17, -8(r1)

  fadd    f16, f16, f16

  fsub    f21, f22, f20
  fdiv    f21, f20, f21
  fmul    f16, f16, f21
  fadd    f17, f17, f16

  ; epart.f*x;
  lfd     f20, 0(r19)
  fmul    f20, f20, f17
  stfd    f20, 0(r19)
```

## Notes
- x87 backends compute `exp(x)` via `fldl2e` + `f2xm1` + `fscale` (and check `fstsw ax` for invalid-op).
- aarch64/ppc64le evaluate an inline rational polynomial using `fm_exp2_p`/`fm_exp2_q` constants from `%CORE_MATH_TABLE` (table laid out at the top of `asm/<arch>/core60.asm`).
- RISC backends require a TOC/page-relative load to reach `CORE_MATH_TABLE`; the prologue varies by OS (`_MAC` vs `_LNX`/`_FREEBSD`).
- Same family as `fln`/`fsin`/`fcos`/`farctan` -- all use the same MATH_TABLE base on RISC.
- x87 backends set `edx = 1` on success, `edx = 0` on FPU error -- caller may inspect this status.
