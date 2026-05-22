# 0x1C -- xlcmp

- **Category:** SingleOp
- **Enum:** `ByteCode::XLCmp`
- **Operand(s):** (none)
- **Reads:** `long:index`, `[acc]` (8 bytes)
- **Writes:** `COMP.EQ`, `COMP.LT`

## Semantics
`long:temp := long:[acc]; COMP.EQ := (long:index == long:temp); COMP.LT := (long:index < long:temp)` -- signed 64-bit compare of `index` with the first qword of `acc`. 32-bit form is `0x0D xcmp`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %1Ch`

```asm
  cmp  [rbx], rdx
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %1Ch`

```asm
  push  eax
  push  edx

  mov   edi, eax
  xor   eax, eax
  sub   edi, [ebx]
  sbb   edx, [ebx+4]
  sets  ah
  or    edx, edi
  setz  al
  mov   ecx, 1
  cmp   eax, ecx

  pop   edx
  pop   eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %1Ch`

```asm
  ldr     x14, [x10]
  cmp     x14, x9
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %1Ch`

```asm
  ld      r18, 0(r15)
  cmpd    r18, r14
```

## Notes
- 64-bit version of `xcmp` (0x0D). Signed compare of `index` against the qword at `[acc]`.
- x32 implementation is a multi-instruction emulation: subtract-with-borrow on the two halves, then synthesise both `EQ` and `LT` flags into a packed byte. Other arches use single-instruction 64-bit cmp.
- Preserves `acc`, `index`, `sp[0]`, `sp[1]`. Clobbers COMP flags and scratch (`edi`, `ecx`, `eax` on x32).
