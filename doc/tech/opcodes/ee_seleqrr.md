# 0xEE -- seleq

- **Category:** TripleOp
- **Enum:** `ByteCode::SelEqRR`
- **Operand(s):** `seleq r1, r2` (R+R)

## Semantics
`acc := COMP.EQ ? r1 : r2` -- conditional move based on the equality flag.

## JIT (amd64)
**Selection rule:** R+R command -- both operand ref masks (`mskXxx`) participate in slot selection. The JIT materialises each pointer in its proper kind (code ref, data ref, raw constant) before the conditional move.

**Template:** `asm/amd64/core60.asm` `inline %0EEh`
```asm
mov   rax, __ptr64_1
mov   rbx, __ptr64_2
cmovz rbx, rax
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0EEh`
```asm
mov   eax, __ptr32_1
mov   ebx, __ptr32_2
cmovz ebx, eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0EEh`
```asm
movz    x11, __ptr32lo_1
movz    x12, __ptr32lo_2
movk    x11, __ptr32hi_1, lsl #16
movk    x12, __ptr32hi_2, lsl #16

csel    x10, x11, x12, eq
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0EEh`
```asm
ld      r16, toc_code(r2)
addis   r17, r16, __xdisp32hi_1
addis   r18, r16, __xdisp32hi_2
addi    r17, r17, __xdisp32lo_1
addi    r18, r18, __xdisp32lo_2

iseleq  r15, r17, r18
```

## Notes
- Branchless conditional select on the equality flag -- typically the result of a preceding `cmp`/`icmp`.
- amd64/x32 use `cmovz`; aarch64 uses `csel ..., eq`; ppc64le uses `iseleq`.
- Both `r1` and `r2` may be different ref kinds (code vs data); the JIT picks an appropriate slot per the R+R mask pair.
