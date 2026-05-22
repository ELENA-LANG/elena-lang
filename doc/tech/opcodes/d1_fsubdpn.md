# 0xD1 -- fsub dp

- **Category:** TripleOp
- **Enum:** `ByteCode::FSubDPN`
- **Operand(s):** `fsub dp:disp, n` (n = 8 only)
- **Reads:** `sp[0]` (8 bytes), `dp[disp]` (8 bytes)
- **Writes:** `dp[disp]` (8 bytes)

## Semantics
`temp := double:sp[0]; dp[disp] -= temp` -- double-precision in-place subtract at the data-frame slot.

## JIT (amd64)
**Selection rule:** dispatched through `loadDPNOp` -> `retrieveICode(scope, arg2)`. With `n = 8` only, the picker selects slot 4 -- a single effective variant. Frame displacement is converted via `getFPOffset`.

**Template:** x87 `fld / fsub / fstp`.

```asm
mov  rsi, r10
lea  rdi, [rbp + __arg32_1]

fld   qword ptr [rdi]
fsub  qword ptr [rsi]
fstp  qword ptr [rdi]
```

## JIT (x32)
**Template:** same x87 sequence; `esi` is the sp[0] shadow.

```asm
lea  edi, [ebp + __arg32_1]

fld   qword ptr [edi]
fsub  qword ptr [esi]
fstp  qword ptr [edi]
```

## JIT (aarch64)
**Template:** NEON `fsub d, d, d` -- note the operand order yields `dp - sp[0]`.

```asm
add     x19, x29, __arg12_1
ldr     d17, [x0]
ldr     d18, [x19]
fsub    d17, d18, d17     ; sp[0] is in d17, dp[disp] in d18
str     d17, [x19]
```

## JIT (ppc64le)
**Template:** `fsub f, f, f` -- operand order preserves `dp - sp[0]`.

```asm
addi    r19, r31, __arg16_1
lfd     f17, 0(r3)
lfd     f18, 0(r19)
fsub    f17, f18, f17
stfd    f17, 0(r19)
```

## Notes
- `n = 8` (double) only.
- Same shape as `fadd dp` (0xD0) but the subtraction is non-commutative -- the backends arrange operands so the result is `dp -= sp[0]`.
- aarch64 uses `fsub d17, d18, d17` ordering; ppc64le mirrors with `fsub f17, f18, f17` -- both compute `dp - sp[0]`.
- The slot at `dp[disp]` is both source and destination.
