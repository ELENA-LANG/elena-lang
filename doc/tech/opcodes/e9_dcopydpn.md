# 0xE9 -- dcopy dp

- **Category:** TripleOp
- **Enum:** `ByteCode::DCopyDPN`
- **Operand(s):** `dcopy dp:disp, n`

## Semantics
Copy `n * index` bytes from `[sp[0]]` to `dp[disp]`. Variant of `0xE0 copy dp` where the byte count is `n * index`.

## JIT (amd64)
**Selection rule:** `loadIROp` -> `retrieveIRCode(scope, arg1, arg2)` (`jitcompiler.cpp:2435-2459`). **Dual-argument** 10-slot rule: `arg1  in  {1,2,3,4}` selects a base slot in `{2,4,6,8}` (i.e. `base = 2*arg1`); if `arg2 == 0` then `base + 1` is used instead. This is unusual -- most opcodes encode width in a single argument; here the (n, i) pair both drive the variant.

**Template:** `asm/amd64/core60.asm` `inline %0E9h`
```asm
mov  rsi, r10
lea  rdi, [rbp + __arg32_1]
mov  ecx, __n_2
imul ecx, edx
rep  movsb
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0E9h`
```asm
mov  eax, esi
lea  edi, [ebp + __arg32_1]
mov  ecx, __n_2
imul ecx, edx
rep  movsb
mov  esi, eax
```

`esi` is the live `sp[0]` register and must be saved/restored across `rep movsb`.

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0E9h`
```asm
mov     x11, __n16_2
mov     x12, x0
mul     x11, x11, x9        ; index in x9
add     x13, x29, __arg12_1

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 1
  ldrb    w14, [x12], #1
  strb    w14, [x13], #1
  b       labLoop

labEnd:
```

## Compiler behaviour
The dual-arg variant selection `retrieveIRCode(scope, arg1, arg2)` (`jitcompiler.cpp:2435-2459`) is what makes `dcopy` different from plain `copy` (0xE0): the JIT inspects both `n` (element size, arg2) and `i` (element count / kind tag, arg1) and picks one of 10 inline slots. The element count multiplier `index` is supplied via a live register (`edx`/`x9`/`r14`) at execution time -- the inline body multiplies `n * index` to get the byte count.

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0E9h`
```asm
li      r16, __n16_2
addi    r18, r31, __arg16_1
mulld   r16, r16, r14       ; index in r14
mr      r19, r3

labLoop:
  cmpwi   r16,0
  beq     labEnd
  ld      r17, 0(r19)
  addi    r16, r16, -1
  stb     r17, 0(r18)
  addi    r18, r18, 1
  addi    r19, r19, 1
  b       labLoop

labEnd:
```

## Notes
- Differs from plain `copy` (0xE0) in that the byte count is `n * index` where `index` lives in a runtime register (`edx`/`x9`/`r14`).
- Variant selection is dual-argument (`arg1`, `arg2`) -- `retrieveIRCode` yields one of 10 inline slots.
- amd64/x32 use `rep movsb` after computing `ecx = n * index`; aarch64/ppc64le use a byte-by-byte loop.
- x32 must preserve `esi` (live `sp[0]`) across `rep movsb`.
