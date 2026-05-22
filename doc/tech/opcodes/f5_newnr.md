# 0xF5 -- newn

- **Category:** TripleOp
- **Enum:** `ByteCode::NewNR`
- **Operand(s):** `newn n, r` (R-cmd)

## Semantics
Allocate a new object of `n` bytes with class `r`. Calls `%GC_ALLOC` and fills the header. Differs from `new` (`0xF4`) in that the size is given in bytes rather than fields.

## JIT (amd64)
**Selection rule:** R-cmd -- variant by `mskVMTRef` shape of `r`. Calls `%GC_ALLOC` with size in bytes (`n`) rather than fields; the size stored in the header is the raw byte count.

**Template:** `asm/amd64/core60.asm` `inline %0F5h`
```asm
mov  ecx, __arg32_1
call %GC_ALLOC

mov  ecx, __n_1
mov  rax, __ptr64_2
mov  [rbx - elVMTOffset], rax
mov  dword ptr [rbx - elSizeOffset], ecx
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0F5h`
```asm
mov  ecx, __arg32_1
call %GC_ALLOC

mov  ecx, __n_1
mov  eax, __ptr32_2
mov  [ebx - elVMTOffset], eax
mov  [ebx - elSizeOffset], ecx
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0F5h`
```asm
mov     x11, __arg16_1
movz    x17, code_ptr32lo : %GC_ALLOC
movk    x17, code_ptr32hi : %GC_ALLOC, lsl #16
blr     x17

movz    x18, __n16lo_1
movk    x18, __n16hi_1, lsl #16
movz    x19, __ptr32lo_2
movk    x19, __ptr32hi_2, lsl #16
sub     x20, x10, elVMTOffset
str     x19, [x20]
str     w18, [x20, #12]!
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0F5h`
```asm
li      r18, __arg16_1
ld      r12, toc_alloc(r2)
mtctr   r12
bctrl

li      r18, __n16lo_1
addis   r18, r18, __n16hi_1

ld      r17, toc_rdata(r2)
addis   r17, r17, __disp32hi_2
addi    r17, r17, __disp32lo_2
std     r18, -elSizeOffset(r15)
std     r17, -elVMTOffset(r15)
```

## Notes
- Like `new` (0xF4) but the size is in bytes -- used for byte-array / string / struct-shaped objects.
- aarch64/ppc64le materialise the 32-bit byte count via `movz/movk` (or `li/addis`) because `n` may exceed 16 bits.
- Allocates from YG via `%GC_ALLOC`.
- Header layout matches `new`: VMT at `-elVMTOffset`, raw byte size at `-elSizeOffset`.
