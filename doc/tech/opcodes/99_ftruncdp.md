# 0x99 -- ftrunc dp

- **Category:** DoubleOp
- **Enum:** `ByteCode::FTruncDP`
- **Operand(s):** `ftrunc dp:disp`
- **Reads:** `sp[0]` (pointer to source double), `fp`
- **Writes:** `dp[disp]` (qword double)
- **Side effects:** x86 backends save/restore the FPU control word so truncation is unaffected by the ambient rounding mode; scratch registers clobbered.

## Semantics
`double:tmp << sp[0]; double:dp[disp] := trunc(tmp)` -- load a double from the buffer at `sp[0]`, truncate toward zero, store the result at `dp[disp]`. Compare with `frounddp` (0x9F) which rounds to nearest.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %099h`

```asm
  mov   rsi, r10
  lea   rdi, [rbp + __arg32_1]

  mov   ecx, 0
  fld   qword ptr [rsi]

  push  rcx                ; reserve space on stack
  fstcw word ptr [rsp]     ; get current control word
  mov   rdx, [rsp]
  or    dx,0c00h           ; code it for truncating
  push  rdx
  fldcw word ptr [rsp]    ; change rounding code of FPU to truncate

  frndint                  ; truncate the number
  pop   rdx                ; remove modified CW from CPU stack
  fldcw word ptr [rsp]     ; load back the former control word
  pop   rdx                ; clean CPU stack

  fstsw ax                 ; retrieve exception flags from FPU
  shr   al,1               ; test for invalid operation
  jc    short labErr       ; clean-up and return error

labSave:
  fstp  qword ptr [rdi]    ; store result
  jmp   short labEnd

labErr:
  ffree st(1)

labEnd:
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %099h`

```asm
  lea   edi, [ebp + __arg32_1]

  mov   ecx, 0
  fld   qword ptr [esi]

  push  ecx                ; reserve space on stack
  fstcw word ptr [esp]     ; get current control word
  mov   edx, [esp]
  or    dx,0c00h           ; code it for truncating
  push  edx
  fldcw word ptr [esp]    ; change rounding code of FPU to truncate

  frndint                  ; truncate the number
  pop   edx                ; remove modified CW from CPU stack
  fldcw word ptr [esp]     ; load back the former control word
  pop   edx                ; clean CPU stack

  fstsw ax                 ; retrieve exception flags from FPU
  shr   al,1               ; test for invalid operation
  jc    short labErr       ; clean-up and return error

labSave:
  fstp  qword ptr [edi]    ; store result
  jmp   short labEnd

labErr:
  ffree st(1)

labEnd:
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %099h`

```asm
  add     x19, x29, __arg12_1
  ldr     d18, [x0]
  frintz  d18, d18
  str     d18, [x19]
```

`frintz` rounds-toward-zero in place -- no CW save/restore needed.

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %099h`

```asm
  addi    r19, r31, __arg16_1

  lfd     f17, 0(r3)
  friz    f17, f17
  stfd    f17, 0(r19)
```

`friz` is the round-toward-zero instruction.

## Notes
- Twin of `frounddp` (0x9F); on x86 the only difference is the CW patch (`or 0C00h` here vs `and 0F3FFh` for round-to-nearest).
- Source loaded from `sp[0]`; destination is a double at `dp[disp]`.
- x86 saves/restores the FPU control word so the ambient rounding mode is preserved.
- On invalid operations the x86 path takes the `labErr` branch which frees the FPU stack instead of storing.
