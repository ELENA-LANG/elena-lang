# 0x9F -- fround dp

- **Category:** DoubleOp
- **Enum:** `ByteCode::FRoundDP`
- **Operand(s):** `fround dp:disp`
- **Reads:** `sp[0]` (pointer to source double), `fp`
- **Writes:** `dp[disp]` (qword double)
- **Side effects:** x86 backends save/restore the FPU control word so rounding-to-nearest is enforced; scratch registers clobbered.

## Semantics
`double:tmp << sp[0]; double:dp[disp] := round(tmp)` -- load a double from `sp[0]`, round to nearest (banker's rounding via the FPU default), store the rounded double at `dp[disp]`. Twin of `ftruncdp` (0x99); on x86 the only difference is the CW patch (`and 0F3FFh` here, `or 0C00h` there).

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %09Fh`

```asm
  lea   rdi, [rbp + __arg32_1]
  mov   rax, r10

  mov   ecx, 0
  fld   qword ptr [rax]

  push  rcx                ; reserve space on stack
  fstcw word ptr [rsp]     ; get current control word

  mov   rdx, [rsp]
  and   dx,0F3FFh          ; code it for code it for rounding
  push  rdx
  fldcw word ptr [rsp]     ; change rounding code of FPU to truncate

  frndint                  ; round the number
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
**Template:** `asm/x32/core60.asm` `inline %09Fh`

```asm
  lea   edi, [ebp + __arg32_1]

  mov   ecx, 0
  fld   qword ptr [esi]

  push  ecx                ; reserve space on stack
  fstcw word ptr [esp]     ; get current control word

  mov   edx, [esp]
  and   dx,0F3FFh          ; code it for code it for rounding
  push  edx
  fldcw word ptr [esp]     ; change rounding code of FPU to truncate

  frndint                  ; round the number
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
**Template:** `asm/aarch64/core60.asm` `inline %9Fh`

```asm
  add     x19, x29, __arg12_1
  ldr     d18, [x0]
  frintn  d18, d18
  str     d18, [x19]
```

`frintn` rounds to nearest in place -- no CW save/restore needed.

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %09Fh`

```asm
  addi    r19, r31, __arg16_1

  lfd     f17, 0(r3)
  frin    f17, f17
  stfd    f17, 0(r19)
```

`frin` is the round-to-nearest instruction.

## Notes
- Twin of `ftruncdp` (0x99); on x86 the only difference is the CW patch (`and 0F3FFh` here vs `or 0C00h` for truncate).
- Source loaded from `sp[0]`; destination is a double at `dp[disp]`.
- Hardware round-to-nearest (banker's rounding on x86 via the FPU default).
- On invalid operations the x86 path takes the `labErr` branch which frees the FPU stack instead of storing.
