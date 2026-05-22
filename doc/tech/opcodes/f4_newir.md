# 0xF4 -- new

- **Category:** TripleOp
- **Enum:** `ByteCode::NewIR`
- **Operand(s):** `new i, r` (R-cmd)

## Semantics
Allocate a new object with `i` fields and class `r`. Calls `%GC_ALLOC` with size in `ecx`, then writes header: VMT pointer (`r`) at `-elVMTOffset`, size (`i` fields) at `-elSizeOffset`. Result in `acc`.

## JIT (amd64)
**Selection rule:** R-cmd -- variant by the `mskVMTRef` shape of `r` (class ref vs. inline VMT). Each slot calls `%GC_ALLOC` with the computed byte size (`i x ptr_size + header`); the JIT writes the VMT pointer into the new object header before returning to the inline body.

**Template:** `asm/amd64/core60.asm` `inline %0F4h`
```asm
mov  ecx, __arg32_1
call %GC_ALLOC

mov  ecx, __n_1
mov  rax, __ptr64_2
mov  dword ptr [rbx - elSizeOffset], ecx
mov  [rbx - elVMTOffset], rax
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0F4h`
```asm
mov  ecx, __arg32_1
call %GC_ALLOC

mov  ecx, __n_1
mov  eax, __ptr32_2
mov  [ebx - elSizeOffset], ecx
mov  [ebx - elVMTOffset], eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0F4h`
```asm
mov     x11, __arg16_1
movz    x17, code_ptr32lo : %GC_ALLOC
movk    x17, code_ptr32hi : %GC_ALLOC, lsl #16
blr     x17

mov     x18, __n16_1
movz    x19, __ptr32lo_2
movk    x19, __ptr32hi_2, lsl #16
sub     x20, x10, elVMTOffset
str     x19, [x20]
str     w18, [x20, #12]!
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0F4h`
```asm
li      r18, __arg16_1
ld      r12, toc_alloc(r2)
mtctr   r12
bctrl

li      r18, __n16_1
ld      r17, toc_rdata(r2)
addis   r17, r17, __disp32hi_2
addi    r17, r17, __disp32lo_2
std     r18, -elSizeOffset(r15)
std     r17, -elVMTOffset(r15)
```

## Notes
- Calls `%GC_ALLOC` with computed byte size = `i x ptr_size + header`.
- VMT pointer is written into the new object header at `-elVMTOffset`; the field count goes into `-elSizeOffset`.
- Allocates from the young generation (YG) -- contrast with `xnewn` (0xE7) which uses PERM.
- Variant slot is picked by the `mskVMTRef` shape of `r`.
