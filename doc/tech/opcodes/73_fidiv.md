# 0x73 -- fidiv

- **Category:** SingleOp
- **Enum:** `ByteCode::FIDiv`
- **Operand(s):** (none)
- **Reads:** `acc` (pointer to double), `sp[0]` (pointer to int)
- **Writes:** `[acc]` (double)

## Semantics
In-place double-by-int divide: `double:[acc] /= int:[sp[0]]`. The 32-bit integer at the address held in `sp[0]` is converted to double and used as the divisor of the double at the address held in `acc`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %73h`

```asm
  mov  rax, r10
  fld   qword ptr [rbx]
  fild  dword ptr [rax]
  fdivp
  fstp  qword ptr [rbx]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %73h`

```asm
  fld   qword ptr [ebx]
  fild  [esi]
  fdivp
  fstp  qword ptr [ebx]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %73h`

```asm
  ldrsw   x19, [x0]
  ldr     d17, [x10]
  scvtf   d18, x19
  fdiv    d17, d17, d18
  str     d17, [x10]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %73h`

```asm
  lfiwax  f18, r3
  fcfid   f18, f18
  lfd     f17, 0(r15)
  fdiv    f17, f17, f18
  stfd    f17, 0(r15)
```

## Notes
- Same shape as `fiadd` (0x70) / `fisub` (0x71) / `fimul` (0x72) but emits the divide.
- Division by zero: x87/aarch64/ppc64le default to producing +/-Inf or NaN without trapping; SSE/MXCSR settings may differ on hosts that have switched FP modes.
- amd64 uses `fdivp` (ST(1)/ST(0), then pop) -- operand order matters; the int divisor is on top of the stack.
- aarch64 `fdiv d17, d17, d18` divides lhs/rhs in the natural order; ppc64le mirrors that.
- `index` and stack are preserved; only `[acc]` is updated.
