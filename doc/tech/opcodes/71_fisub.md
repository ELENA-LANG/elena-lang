# 0x71 -- fisub

- **Category:** SingleOp
- **Enum:** `ByteCode::FISub`
- **Operand(s):** (none)
- **Reads:** `acc` (pointer to double), `sp[0]` (pointer to int)
- **Writes:** `[acc]` (double)

## Semantics
In-place double-from-int subtract: `double:[acc] -= int:[sp[0]]`. The 32-bit integer at the address held in `sp[0]` is converted to double and subtracted from the double at the address held in `acc`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %71h`

```asm
  mov  rax, r10
  fld   qword ptr [rbx]
  fild  [rax]
  fsubp
  fstp  qword ptr [rbx]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %71h`

```asm
  fld   qword ptr [ebx]
  fild  [esi]
  fsubp
  fstp  qword ptr [ebx]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %71h`

```asm
  ldrsw   x19, [x0]
  ldr     d17, [x10]
  scvtf   d18, x19
  fsub    d17, d17, d18
  str     d17, [x10]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %71h`

```asm
  lfiwax  f18, r3
  fcfid   f18, f18
  lfd     f17, 0(r15)
  fsub    f17, f18, f17
  stfd    f17, 0(r15)
```

Note: the ppc64le body computes `f18 - f17` (i.e. `int - double`); the other backends compute `double - int`. Likely a bug.

## Notes
- Mirror of `fiadd` (0x70) but uses subtraction; shares the same load/convert/op/store skeleton.
- **Known divergence**: ppc64le computes `int - double` rather than `double - int` -- verify before relying on POWER results.
- amd64/x32 rely on `fsubp` semantics (ST(1) - ST(0), then pop); operand order matters.
- aarch64 emits the canonical `fsub d17, d17, d18` (lhs - rhs).
- `index` and stack are preserved; only the qword at `[acc]` changes.
