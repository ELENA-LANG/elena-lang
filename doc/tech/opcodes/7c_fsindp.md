# 0x7C -- fsin dp

- **Category:** DoubleOp
- **Enum:** `ByteCode::FSinDP`
- **Operand(s):** `disp` (frame-pointer displacement)
- **Reads:** `sp[0]` (pointer to double)
- **Writes:** `double:dp[disp]`
- **Side effects:** clobbers FPU stack (x87 backends), reads `CORE_MATH_TABLE` (aarch64)

## Semantics
Computes the sine of the double pointed to by `sp[0]`, storing the result at `dp[disp]`: `double:dp[disp] := sin(double:[sp[0]])`. x87 backends use the `fsin` instruction with a `fprem`/`2pi` reduction loop. The aarch64 backend reduces the angle by `PI_4`, then dispatches through an 8-entry octant jump table, evaluating either a sine or cosine polynomial against the `sin1a*` / `cos1a*` constant blocks at `CORE_MATH_TABLE + 192` / `+224`. The ppc64le backend is not implemented.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %7Ch`

```asm
  mov   rax, r10
  lea   rdi, [rbp + __arg32_1]
  fld   qword ptr [rax]
  fldpi
  fadd  st(0),st(0)       ; ->2pi
  fxch

lReduce:
  fprem                   ; reduce the angle
  fsin
  fstsw ax                ; retrieve exception flags from FPU
  shr   al,1              ; test for invalid operation
  ; jc    short lErr        ; clean-up and return error
  sahf                    ; transfer to the CPU flags
  jpe   short lReduce     ; reduce angle again if necessary
  fstp  st(1)             ; get rid of the 2pi

  fstp  qword ptr [rdi]    ; store result
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %7Ch`

```asm
  lea   edi, [ebp + __arg32_1]
  fld   qword ptr [esi]
  fldpi
  fadd  st(0),st(0)       ; ->2pi
  fxch

lReduce:
  fprem                   ; reduce the angle
  fsin
  fstsw ax                ; retrieve exception flags from FPU
  shr   al,1              ; test for invalid operation
  ; jc    short lErr        ; clean-up and return error
  sahf                    ; transfer to the CPU flags
  jpe   short lReduce     ; reduce angle again if necessary
  fstp  st(1)             ; get rid of the 2pi

  fstp  qword ptr [edi]    ; store result
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %7Ch`

```asm
labStart:
  ldr     d0, [x0]           ; x (d0)

  add     x19, x29, __arg12_1 ; dest (x19)

#if _MAC
  adrp    x20, rdata_page : %CORE_MATH_TABLE
  add     x20, x20, rdata_pageoff : %CORE_MATH_TABLE
#elif (_LNX || _FREEBSD)
  movz    x20, rdata_ptr32lo : %CORE_MATH_TABLE
  movk    x20, rdata_ptr32hi : %CORE_MATH_TABLE, lsl #16
#endif

  ldr     d1, [x20, #176] ; d1 <- PI_4
  adr     x23, sin2a0     ; origin of jump table

  fdiv    d2, d0, d1     ; d2 <- x / PI_4
  frintm  d2, d2         ; d2 <- floor (d2)
  fmsub   d0, d1, d2, d0 ; d0 <- x mod PI_4

  fcvtas  x21, d2
  and     x21, x21, #7
  add     x23, x23, x21, lsl #4
  br      x23

sin2a0:
  b       sin1a
  nop
  nop
  nop
sin2a1:
  fsub    d0, d1, d0
  b       cos1a
  nop
  nop
sin2a2:
  b       cos1a
  nop
  nop
  nop
sin2a3:
  fsub    d0, d1, d0
  b       sin1a
  nop
  nop
sin2a4:
  fneg    d0, d0
  b       sin1a
  nop
  nop
sin2a5:
  fsub    d0, d1, d0
  b       cos1a_neg
  nop
  nop
sin2a6:
  b       cos1a_neg
  nop
  nop
  nop
sin2a7:
  fsub    d0, d0, d1

sin1a:
#if _MAC
  adrp    x20, rdata_page : %CORE_MATH_TABLE             ; sin1a1
  add     x20, x20, rdata_pageoff : %CORE_MATH_TABLE
#elif (_LNX || _FREEBSD)
  movz    x20, rdata_ptr32lo : %CORE_MATH_TABLE             ; sin1a1
  movk    x20, rdata_ptr32hi : %CORE_MATH_TABLE, lsl #16
#endif
  add     x20, x20, #192

  ldp     d4, d5, [x20], #16
  ldp     d6, d7, [x20]

  fmul    d1,d0,d0 ; d1 <- x^2
  fmul    d2,d1,d0 ; d2 <- x^3
  fmul    d3,d2,d1 ; d3 <- x^5
  fmul    d1,d1,d3 ; d1 <- x^7
  fmadd   d0,d5,d2,d0
  fmadd   d0,d6,d3,d0
  fmadd   d0,d7,d1,d0
  b       labEnd

cos1a_neg:
#if _MAC
  adrp    x20, rdata_page : %CORE_MATH_TABLE             ; cos1a1
  add     x20, x20, rdata_pageoff : %CORE_MATH_TABLE
#elif (_LNX || _FREEBSD)
  movz    x20, rdata_ptr32lo : %CORE_MATH_TABLE             ; cos1a1
  movk    x20, rdata_ptr32hi : %CORE_MATH_TABLE, lsl #16
#endif
  add     x20, x20, #224

  ldp     d4, d5, [x20], #16
  ldp     d6, d7, [x20]

  fmul    d1,d0,d0 ; d1 <- x^2
  fmul    d2,d1,d1 ; d2 <- x^4
  fmul    d3,d2,d1 ; d3 <- x^6
  fmov    d0,d4 ; d0 <- 1
  fmadd   d0,d1,d5,d0
  fmadd   d0,d2,d6,d0
  fmadd   d0,d3,d7,d0
  fneg    d0, d0
  b       labEnd

cos1a:
#if _MAC
  adrp    x20, rdata_page : %CORE_MATH_TABLE             ; cos1a1
  add     x20, x20, rdata_pageoff : %CORE_MATH_TABLE
#elif (_LNX || _FREEBSD)
  movz    x20, rdata_ptr32lo : %CORE_MATH_TABLE             ; cos1a1
  movk    x20, rdata_ptr32hi : %CORE_MATH_TABLE, lsl #16
#endif
  add     x20, x20, #224

  ldp     d4,d5,[x20],16
  ldp     d6,d7,[x20]

  fmul    d1,d0,d0 ; d1 <- x^2
  fmul    d2,d1,d1 ; d2 <- x^4
  fmul    d3,d2,d1 ; d3 <- x^6
  fmov    d0,d4 ; d0 <- 1
  fmadd   d0,d1,d5,d0
  fmadd   d0,d2,d6,d0
  fmadd   d0,d3,d7,d0

labEnd:
  str     d0, [x19]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %7Ch`

*Not implemented in this backend.*

## Notes
- x87 backends use the hardware `fsin` with a `fprem`/2pi reduction loop (PE flag in `fstsw ax` signals "needs more reduction").
- aarch64 reduces the argument by PI_4, splits into 8 octants, and dispatches through a 16-byte-aligned jump table into `sin1a` (odd octants) or `cos1a`/`cos1a_neg` (even octants) polynomial blocks.
- Coefficient tables live at `%CORE_MATH_TABLE + 192` (sin1a coefficients) and `+224` (cos1a coefficients); PI_4 sits at offset 176.
- ppc64le has **no implementation** -- callers on POWER must route through a runtime helper or avoid this opcode.
- `acc`, `index`, and stack pointer are preserved; only the slot at `dp[disp]` is written.
