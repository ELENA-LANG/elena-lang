# 0xD2 -- fmul dp

- **Category:** TripleOp
- **Enum:** `ByteCode::FMulDPN`
- **Operand(s):** `fmul dp:disp, n` (n = 8 only)
- **Reads:** `sp[0]` (8 bytes), `dp[disp]` (8 bytes)
- **Writes:** `dp[disp]` (8 bytes)

## Semantics
`temp := double:sp[0]; dp[disp] *= temp` -- double-precision in-place multiply.

## JIT (amd64)
**Selection rule:** dispatched through `loadDPNOp` -> `retrieveICode(scope, arg2)`. With `n = 8` only, the picker selects slot 4 -- a single effective variant. Frame displacement transformed via `getFPOffset`.

**Template:** x87 `fmul`.

```asm
mov  rsi, r10
lea  rdi, [rbp + __arg32_1]

fld   qword ptr [rdi]
fmul  qword ptr [rsi]
fstp  qword ptr [rdi]
```

## JIT (x32)
**Template:** identical, with `esi` providing sp[0].

```asm
lea  edi, [ebp + __arg32_1]

fld   qword ptr [edi]
fmul  qword ptr [esi]
fstp  qword ptr [edi]
```

## JIT (aarch64)
**Template:** NEON `fmul`.

```asm
add     x19, x29, __arg12_1
ldr     d17, [x0]
ldr     d18, [x19]
fmul    d17, d17, d18
str     d17, [x19]
```

## JIT (ppc64le)
**Template:** PPC FPU `fmul`.

```asm
addi    r19, r31, __arg16_1
lfd     f17, 0(r3)
lfd     f18, 0(r19)
fmul    f17, f17, f18
stfd    f17, 0(r19)
```

## Notes
- `n = 8` (double) only.
- Commutative -- operand order does not matter (unlike `fsub`/`fdiv`).
- Read-modify-write on `dp[disp]`.
- amd64/x32 use x87 (`fmul`); aarch64/ppc64le use native NEON / FPU.
