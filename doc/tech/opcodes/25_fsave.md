# 0x25 -- fsave

- **Category:** SingleOp
- **Enum:** `ByteCode::FSave`
- **Operand(s):** (none)
- **Reads:** index, acc
- **Writes:** qword (double) at `[acc]`
- **Side effects:** uses x87/FPU scratch on x86; transient stack slot on ppc64le

## Semantics
Converts integer `index` to IEEE-754 double and stores as a qword at `[acc]`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %25h`

```asm
  push rdx
  fild dword ptr [rsp]
  fstp qword ptr [rbx]
  add  rsp, 8
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %25h`

```asm
  push edx
  fild [esp]
  fstp qword ptr [ebx]
  add  esp, 4
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %25h`

```asm
  scvtf   d17, x9
  str     d17, [x10]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %25h`

```asm
  extsw   r17, r14
  std     r17, 0(r1)
  lfd     f17, 0(r1)
  fcfid   f17, f17
  stfd    f17, 0(r15)
```

## Notes
- Implicit int-to-double conversion of `index` followed by a qword store at `[acc]`.
- amd64/x32 round-trip through the x87 stack (`fild`/`fstp`) -- the FPU control word state matters; ensure default rounding mode.
- aarch64 uses the dedicated `scvtf` (signed-convert-to-float) instruction; ppc64le emulates with `extsw` + memory bounce + `fcfid`.
- ppc64le uses `r1` (stack pointer) as a transient scratch slot -- make sure the redzone is respected.
- For the 64-bit-index variant see `xfsave` (0x30) which handles full 64-bit source values.
