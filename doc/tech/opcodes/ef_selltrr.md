# 0xEF -- sellt

- **Category:** TripleOp
- **Enum:** `ByteCode::SelLtRR`
- **Operand(s):** `sellt r1, r2` (R+R)

## Semantics
`acc := COMP.LT ? r1 : r2` -- conditional move based on the less-than flag (signed).

## JIT (amd64)
**Selection rule:** R+R command -- variant chosen by both ref masks (same scheme as `seleq`).

**Template:** `asm/amd64/core60.asm` `inline %0EFh`
```asm
mov   rax, __ptr64_1
mov   rbx, __ptr64_2
cmovl rbx, rax
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0EFh`
```asm
mov   eax, __ptr32_1
mov   ebx, __ptr32_2
cmovl ebx, eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0EFh`
```asm
movz    x11, __ptr32lo_1
movz    x12, __ptr32lo_2
movk    x11, __ptr32hi_1, lsl #16
movk    x12, __ptr32hi_2, lsl #16

csel    x10, x11, x12, lt
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0EFh`
```asm
ld      r16, toc_code(r2)
addis   r17, r16, __xdisp32hi_1
addis   r18, r16, __xdisp32hi_2
addi    r17, r17, __xdisp32lo_1
addi    r18, r18, __xdisp32lo_2

isellt  r15, r17, r18
```

## Notes
- Branchless conditional select on the signed less-than flag.
- amd64/x32 use `cmovl`; aarch64 uses `csel ..., lt`; ppc64le uses `isellt`.
- Used to compile `min`/`max`/clamp-style expressions without a branch.
- Pairs with a preceding signed `cmp`/`icmp`.
