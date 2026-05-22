# 0x70 -- fiadd

- **Category:** SingleOp
- **Enum:** `ByteCode::FIAdd`
- **Operand(s):** (none)
- **Reads:** `acc` (pointer to double), `sp[0]` (pointer to int)
- **Writes:** `[acc]` (double)

## Semantics
In-place double-from-int accumulate: `double:[acc] += int:[sp[0]]`. The 32-bit integer at the address held in `sp[0]` is converted to double and added to the double at the address held in `acc`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %70h`

```asm
  mov  rax, r10
  fld   qword ptr [rbx]
  fild  [rax]
  faddp
  fstp  qword ptr [rbx]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %70h`

```asm
  fld   qword ptr [ebx]
  fild  [esi]
  faddp
  fstp  qword ptr [ebx]
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %70h`

```asm
  ldrsw   x19, [x0]
  ldr     d17, [x10]
  scvtf   d18, x19
  fadd    d17, d17, d18
  str     d17, [x10]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %70h`

```asm
  lfiwax  f18, r3
  fcfid   f18, f18
  lfd     f17, 0(r15)
  fadd    f17, f17, f18
  stfd    f17, 0(r15)
```

## Notes
- Reads a 32-bit integer through `sp[0]`, converts to double, and adds to the double at `[acc]` in place.
- amd64/x32 use the x87 stack (`fld`/`fild`/`faddp`/`fstp`) -- implicit conversion done by `fild`.
- aarch64 emits the conversion explicitly via `scvtf`; ppc64le uses `lfiwax` + `fcfid`.
- Family: `fisub` (0x71), `fimul` (0x72), `fidiv` (0x73) share the exact same shape, only the FP op changes.
- The `index` register and stack are preserved; only the qword at `[acc]` is mutated.
