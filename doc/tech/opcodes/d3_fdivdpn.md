# 0xD3 -- fdiv dp

- **Category:** TripleOp
- **Enum:** `ByteCode::FDivDPN`
- **Operand(s):** `fdiv dp:disp, n` (n = 8 only)
- **Reads:** `sp[0]` (8 bytes), `dp[disp]` (8 bytes)
- **Writes:** `dp[disp]` (8 bytes)

## Semantics
`temp := double:sp[0]; dp[disp] /= temp` -- double-precision in-place divide.

## JIT (amd64)
**Selection rule:** dispatched through `loadDPNOp` -> `retrieveICode(scope, arg2)`. With `n = 8` only, the picker selects slot 4 -- a single effective variant. Frame displacement transformed via `getFPOffset`.

**Template:** x87 `fdiv` of memory operand.

```asm
mov  rsi, r10
lea  rdi, [rbp + __arg32_1]

fld   qword ptr [rdi]
fdiv  qword ptr [rsi]
fstp  qword ptr [rdi]
```

## JIT (x32)
**Template:** identical, with `esi` for sp[0].

```asm
lea  edi, [ebp + __arg32_1]

fld   qword ptr [edi]
fdiv  qword ptr [esi]
fstp  qword ptr [edi]
```

## JIT (aarch64)
**Template:** NEON `fdiv` -- note operand order to compute `dp / sp[0]`.

```asm
add     x19, x29, __arg12_1
ldr     d17, [x0]
ldr     d18, [x19]
fdiv    d18, d18, d17    ; dp[disp] / sp[0]
str     d18, [x19]
```

## JIT (ppc64le)
**Template:** PPC FPU `fdiv`.

```asm
addi    r19, r31, __arg16_1
lfd     f17, 0(r3)
lfd     f18, 0(r19)
fdiv    f18, f18, f17
stfd    f18, 0(r19)
```

## Notes
- `n = 8` (double) only.
- Non-commutative; like `fsub`, the backends arrange operands so the dp[disp] slot is the dividend.
- IEEE-754 division-by-zero produces +/-inf / NaN per the host FPU; no software check.
- aarch64 `fdiv d18, d18, d17` computes `dp / sp[0]`; ppc64le mirrors the order.
