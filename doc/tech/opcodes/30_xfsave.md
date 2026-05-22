# 0x30 -- xfsave

- **Category:** SingleOp
- **Enum:** `ByteCode::XFSave`
- **Operand(s):** (none)
- **Reads:** FPU/SIMD top (on x86), `d0`/`f3` (aarch64/ppc64le), acc
- **Writes:** qword at `[acc]`

## Semantics
Stores the current floating-point top-of-stack / convention scalar as a 64-bit IEEE-754 double at `[acc]`. Used by float-typed return / assign sequences where the value is already in the FP register from a prior op.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %30h`

```asm
  fstp qword ptr [rbx]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %30h`

```asm
  fstp qword ptr [ebx]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %30h`

```asm
  str     d0, [x10]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %30h`

```asm
  stfd    f3, 0(r15)
```

## Notes
- 64-bit (qword) double store at `[acc]` -- the FP-register-source counterpart to `fsave` (0x25).
- Source is the architecture's FP convention scalar: x87 ST(0) on amd64/x32, `d0` on aarch64, `f3` on ppc64le.
- amd64/x32 pop the x87 stack via `fstp` -- verify the FPU stack depth coming in.
- No integer-to-float conversion here; the value is assumed already in the FP register from a prior op.
- `index` register and stack are preserved.
