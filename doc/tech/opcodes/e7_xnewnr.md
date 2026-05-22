# 0xE7 -- xnewn

- **Category:** TripleOp
- **Enum:** `ByteCode::XNewNR`
- **Operand(s):** `xnewn n, r` (R-cmd)

## Semantics
Convert a previously stack-reserved region into an object instance: bump `acc` past the object header, then write the VMT pointer (`r`) and size (`n`) at the negative-offset header slots. No GC allocation -- caller already reserved space.

## JIT (amd64)
**Selection rule:** R-cmd -- the variant is chosen by the `mskXxx` class of `r` (VMT vs. class-symbol vs. message ref). The allocation slot lands in **PERM** (long-lived) rather than the YG nursery, so the object survives across collections.

**Template:** `asm/amd64/core60.asm` `inline %0E7h`
```asm
lea  rbx, [rbx + elObjectOffset]

mov  ecx, __n_1
mov  rax, __ptr64_2
mov  [rbx - elVMTOffset], rax
mov  dword ptr [rbx - elSizeOffset], ecx
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0E7h`
```asm
lea  ebx, [ebx + elObjectOffset]
mov  ecx, __n_1
mov  eax, __ptr32_2
mov  [ebx - elVMTOffset], eax
mov  [ebx - elSizeOffset], ecx
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0E7h`
```asm
add     x10, x10, elObjectOffset
movz    x18, __n16lo_1
movk    x18, __n16hi_1, lsl #16
; load &class r
movz    x19, __ptr32lo_2
movk    x19, __ptr32hi_2, lsl #16
sub     x20, x10, elVMTOffset
str     x19, [x20]
str     w18, [x20, #12]!
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0E7h`
```asm
addi    r15, r15, elObjectOffset

li      r18, __n16lo_1
addis   r18, r18, __n16hi_1

ld      r17, toc_rdata(r2)
addis   r17, r17, __disp32hi_2
addi    r17, r17, __disp32lo_2
std     r18, -elSizeOffset(r15)
std     r17, -elVMTOffset(r15)
```

## Notes
- Allocates in PERM (long-lived heap), not YG nursery -- survives full collections without relocation.
- Typical use: global/static singletons whose lifetime matches the program.
- Header layout: `[-elVMTOffset]` <- VMT/class pointer, `[-elSizeOffset]` <- size word, payload starts at `acc` after `+elObjectOffset` bump.
- `r` is resolved against a class-shape mask family (`mskVMTRef`, etc.); the JIT picks the matching slot.
