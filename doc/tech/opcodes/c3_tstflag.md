# 0xC3 -- tst flag

- **Category:** DoubleOp
- **Enum:** `ByteCode::TstFlag`
- **Operand(s):** `tstflg n` (32-bit mask)
- **Reads:** `acc::VMT::flag`
- **Writes:** `COMP.EQ`

## Semantics
`temp := acc::VMT::flag & n`; `COMP.EQ := (temp == 0)`. Walks two indirections (`acc -> VMT -> flag`) and masks against the immediate.

## JIT (amd64)
**Selection rule:** dispatched through `loadDPNOp` -> `retrieveICode(scope, arg)`. The picker keys on the magnitude of the mask `n`: small masks (<= `mediumForm`) use slot 0; larger 32-bit masks use slot 10 (extended-form). On amd64 a single `test r32, imm32` covers both; aarch64/ppc64le emit a wider materialisation for the 32-bit immediate.

**Template:** chase VMT then flag, `test` against the 32-bit immediate.

```asm
mov  rcx, [rbx - elVMTOffset]
mov  rax, [rcx - elVMTFlagOffset]
test eax, __n_1
```

## JIT (x32)
**Template:** identical structure on the 32-bit GPR set.

```asm
mov  ecx, [ebx - elVMTOffset]
mov  eax, [ecx - elVMTFlagOffset]
test eax, __n_1
```

## JIT (aarch64)
**Template:** two `sub` + `ldr` pairs to chase the two negative offsets; materialise the 32-bit mask via `movz/movk`; `tst` sets NZCV.

```asm
sub     x14, x10, elVMTOffset
ldr     x14, [x14]
sub     x14, x14, elVMTFlagOffset
ldr     x14, [x14]
movz    x11, __n16lo_1
movk    x11, __n16hi_1, lsl #16
tst     x14, x11
```

## JIT (ppc64le)
**Template:** load via negative offsets, build the mask with `addis/addi`, `and.` writes CR0.

```asm
ld      r16, -elVMTOffset(r15)
ld      r16, -elVMTFlagOffset(r16)
li      r17, __n16lo_1
addis   r17, r17, __n16hi_1
and.    r17, r17, r16
```

## Notes
- Used to query class flags (e.g., dispatcher / stateless / structural).
- `EQ == 1` means *all bits in the mask are clear* in the flag word -- typical pattern is `tstflg <bits>; jne <handle>`.
- Walks `acc -> VMT (acc-elVMTOffset) -> flag (VMT-elVMTFlagOffset)` -- two indirections, neither nullable in well-formed bytecode.
- Mask materialisation differs by mask magnitude on aarch64/ppc64le; amd64/x32 always emit a single 32-bit `test`.
