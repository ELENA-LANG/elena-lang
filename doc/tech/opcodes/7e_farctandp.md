# 0x7E -- farctan dp

- **Category:** DoubleOp
- **Enum:** `ByteCode::FArctanDP`
- **Operand(s):** `disp` (frame-pointer displacement)
- **Reads:** `sp[0]` (pointer to double)
- **Writes:** `double:dp[disp]`
- **Side effects:** clobbers FPU stack (x87 backends)

## Semantics
Computes the arctangent of the double pointed to by `sp[0]`, storing the result at `dp[disp]`: `double:dp[disp] := arctan(double:[sp[0]])`. x87 backends use the `fpatan` two-argument form `arctan(x/1)`. The aarch64 and ppc64le backends are not implemented (empty inline bodies).

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %7Eh`

```asm
  mov   rax, r10
  lea   rdi, [rbp + __arg32_1]
  fld   qword ptr [rax]
  fld1
  fpatan                   ; i.e. arctan(Src/1)
  fstp  qword ptr [rdi]    ; store result
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %7Eh`

```asm
  lea   edi, [ebp + __arg32_1]
  fld   qword ptr [esi]
  fld1
  fpatan                   ; i.e. arctan(Src/1)
  fstp  qword ptr [edi]    ; store result
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %7Eh`

*Not implemented in this backend.*

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %7Eh`

*Not implemented in this backend.*

## Notes
- x87 backends use the two-argument `fpatan` with denominator = 1.0, computing `arctan(x/1)` (range `[-pi/2, pi/2]`).
- aarch64 and ppc64le have **no implementation** -- callers on those targets must route to a runtime helper.
- For full `atan2(y, x)` semantics, an additional sequence outside this opcode is required.
- x87 result is left on the FPU top then popped via `fstp`; verify no other FPU state is live across the call.
- `acc`, `index`, and stack pointer are preserved.
