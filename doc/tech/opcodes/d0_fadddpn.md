# 0xD0 -- fadd dp

- **Category:** TripleOp
- **Enum:** `ByteCode::FAddDPN`
- **Operand(s):** `fadd dp:disp, n` (n = 8 only -- double)
- **Reads:** `sp[0] -> [sp[0]]` (8 bytes), `dp[disp]` (8 bytes)
- **Writes:** `dp[disp]` (8 bytes)

## Semantics
`temp := double:sp[0]; dp[disp] += temp`. Double-precision in-place add at the data-frame slot. The n operand is fixed at 8.

## JIT (amd64)
**Selection rule:** dispatched through `loadDPNOp` -> `retrieveICode(scope, arg2)`. Since `n` is fixed at 8 (double) the picker always selects slot 4 -- effectively a single variant. The frame displacement is patched directly via `getFPOffset` so the byte offset in the emitted code reflects the frame layout, not the raw slot index.

**Template:** x87 -- `fld/fadd/fstp`.

```asm
mov  rsi, r10
lea  rdi, [rbp + __arg32_1]

fld   qword ptr [rdi]
fadd  qword ptr [rsi]
fstp  qword ptr [rdi]
```

## JIT (x32)
**Template:** identical x87 sequence; `esi` already holds `sp[0]`.

```asm
lea  edi, [ebp + __arg32_1]

fld   qword ptr [edi]
fadd  qword ptr [esi]
fstp  qword ptr [edi]
```

## JIT (aarch64)
**Template:** NEON `fadd d, d, d`; result written back via `str`.

```asm
add     x19, x29, __arg12_1
ldr     d17, [x0]
ldr     d18, [x19]
fadd    d17, d17, d18
str     d17, [x19]
```

## JIT (ppc64le)
**Template:** standard PowerPC FPU.

```asm
addi    r19, r31, __arg16_1
lfd     f17, 0(r3)
lfd     f18, 0(r19)
fadd    f17, f17, f18
stfd    f17, 0(r19)
```

## Notes
- `n = 8` (double) only; narrower FP widths are not emitted by this branch.
- Commutative; the operand order (sp[0] vs dp[disp]) does not affect the result.
- The slot at `dp[disp]` is both source and destination -- read-modify-write.
- The whole opcode family `0xD0..0xD3` is structurally identical apart from the arithmetic mnemonic.
- amd64/x32 emit x87 (`fadd`); aarch64/ppc64le emit native FPU (`fadd d, d, d` / `fadd f, f, f`).
