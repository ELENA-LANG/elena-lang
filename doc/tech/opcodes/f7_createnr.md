# 0xF7 -- createn

- **Category:** TripleOp
- **Enum:** `ByteCode::CreateNR`
- **Operand(s):** `createn n, r` (R-cmd)

## Semantics
Create an object whose size is `n * [sp[0]]` bytes (dynamic, taken from sp[0] payload). Size is padded to page boundary, allocated via `%GC_ALLOC`, then header is filled with size (or'd with `struct_mask`) and VMT pointer `r`.

## JIT (amd64)
**Selection rule:** R-cmd -- variant by `mskVMTRef` shape of `r` combined with `n`. For `n  in  {1,2,4,8}` (x32 only) the JIT swaps `imul` for a constant shift; the generic slot uses `imul`. `%GC_ALLOC` is called with the page-rounded byte count.

**Template:** `asm/amd64/core60.asm` `inline %0F7h`
```asm
mov  rax, [r10]
mov  ecx, page_ceil
imul eax, __n_1
add  ecx, eax
and  ecx, page_mask
call %GC_ALLOC

mov  rcx, r10
mov  eax, __n_1
mov  ecx, dword ptr [rcx]
imul ecx, eax
or   ecx, struct_mask

mov  rax, __ptr64_2
mov  [rbx - elSizeOffset], rcx
mov  [rbx - elVMTOffset], rax
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0F7h`
```asm
; generic
mov  eax, [esi]
mov  ecx, page_ceil
imul eax, __n_1
add  ecx, eax
and  ecx, page_mask
call %GC_ALLOC

mov  ecx, [esi]
mov  eax, __n_1
imul ecx, eax
mov  eax, __ptr32_2
or   ecx, struct_mask
mov  [ebx - elVMTOffset], eax
mov  [ebx - elSizeOffset], ecx
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%0F7h` | generic | `imul ecx, __n_1` |
| `%2F7h` | 1 | no multiplication |
| `%3F7h` | 2 | `shl ecx, 1` |
| `%5F7h` | 4 | `shl ecx, 2` |
| `%7F7h` | 8 | `shl ecx, 3` |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0F7h`
```asm
ldr     w19, [x0]
movz    x18, __n16_1
mul     x19, x19, x18
add     x19, x19, page_ceil
and     x11, x19, page_mask

movz    x17, code_ptr32lo : %GC_ALLOC
movk    x17, code_ptr32hi : %GC_ALLOC, lsl #16
blr     x17

ldr     w19, [x0]
movz    x18, __n16_1
mul     x18, x19, x18

; adding mask
movz    x19, struct_mask_lo
movk    x19, struct_mask_hi, lsl #16
orr     x18, x18, x19

movz    x19, __ptr32lo_2
movk    x19, __ptr32hi_2, lsl #16
sub     x20, x10, elVMTOffset
str     x19, [x20]
str     w18, [x20, #12]!
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0F7h`
```asm
ld      r12, 0(r3)
li      r10, __n16_1
mulld   r12, r12, r10
addi    r12, r12, page_ceil
andi.   r18, r12, page_mask

ld      r12, toc_alloc(r2)
mtctr   r12
bctrl

ld      r12, 0(r3)
li      r10, __n16_1
mulld   r12, r12, r10

li      r16, struct_mask_lo
addis   r16, r16, struct_mask_hi
or      r18, r12, r16

ld      r17, toc_rdata(r2)
addis   r17, r17, __disp32hi_2
addi    r17, r17, __disp32lo_2
std     r18, -elSizeOffset(r15)
std     r17, -elVMTOffset(r15)
```

## Notes
- Dynamic-sized object create: total size = `n x [sp[0]]` bytes, then page-rounded for `%GC_ALLOC`.
- The stored header size is OR'd with `struct_mask` to flag it as a struct-shaped (raw-bytes) object.
- For element widths `n  in  {1,2,4,8}` the multiplication is replaced by a shift; arbitrary `n` falls through to `imul`/`mulld`.
- Allocates from YG.
