# 0xCE -- xcreate

- **Category:** DoubleOp
- **Enum:** `ByteCode::XCreateR`
- **Operand(s):** `xcreate {prefix}:r` (R-cmd; class reference)
- **Reads:** `sp[0]` (size in fields)
- **Writes:** `acc` (new instance), clobbers `index`

## Semantics
Allocate the object in the permanent (non-collected) heap. Reads the field count from `sp[0]`, multiplies by the pointer width, aligns up to page granularity, calls `GC_ALLOCPERM`, then stores size + VMT into the header.

## JIT (amd64)
**Selection rule:** R-command dispatched through `loadROp` (`jitcompiler.cpp:1256`). Variant by `mskXxx` reference kind of the VMT operand. Critical difference from `new`/`newn` (0xF4/0xF5): allocates via `GC_ALLOCPERM` (PERM region) rather than `GC_ALLOC` (YG / nursery), so the object is never moved or collected.

**Template:** scale field count `*8`, page-align, call into the GC, fill size and VMT.

```asm
mov  rax, [r10]
mov  ecx, page_ceil
shl  eax, 3
add  ecx, eax
and  ecx, page_mask
call %GC_ALLOCPERM

mov  rcx, r10
mov  rax, __ptr64_1
mov  ecx, dword ptr [rcx]
shl  ecx, 3

mov  [rbx - elSizeOffset], rcx
mov  [rbx - elVMTOffset], rax
```

## JIT (x32)
**Template:** scale `*4` (32-bit pointers), same page-align, `GC_ALLOCPERM`, store header.

```asm
mov  eax, [esi]
mov  ecx, page_ceil
shl  eax, 2
add  ecx, eax
and  ecx, page_mask
call %GC_ALLOCPERM

mov  ecx, [esi]
shl  ecx, 2
mov  eax, __ptr32_1
mov  [ebx - elVMTOffset], eax
mov  [ebx - elSizeOffset], ecx
```

## JIT (aarch64)
**Template:** load size, `lsl x19, x19, #3`, align, call `GC_ALLOCPERM` via a materialised function pointer; store VMT + size.

```asm
ldr     w19, [x0]
lsl     x19, x19, #3
add     x19, x19, page_ceil
and     x11, x19, page_mask

; load GC_ALLOCPERM into x17
movz    x17, code_ptr32lo : %GC_ALLOCPERM
movk    x17, code_ptr32hi : %GC_ALLOCPERM, lsl #16
blr     x17

ldr     w19, [x0]
lsl     x18, x19, #3

movz    x19, __ptr32lo_1
movk    x19, __ptr32hi_1, lsl #16
sub     x20, x10, elVMTOffset
str     x19, [x20]
str     w18, [x20, #12]!
```

## JIT (ppc64le)
**Template:** size scaled `*8`, aligned, `GC_ALLOCPERM` reached via the TOC.

```asm
ld      r12, 0(r3)
sldi    r12, r12, 3
addi    r12, r12, page_ceil
andi.   r18, r12, page_mask

ld      r12, toc_allocperm(r2)
mtctr   r12
bctrl

ld      r12, 0(r3)
sldi    r18, r12, 3

ld      r17, toc_rdata(r2)
addis   r17, r17, __disp32hi_1
addi    r17, r17, __disp32lo_1
std     r18, -elSizeOffset(r15)
std     r17, -elVMTOffset(r15)
```

## Notes
- Allocates in **PERM** (`GC_ALLOCPERM`), not YG -- the object is never moved or collected.
- Differs from `new` / `newn` (0xF4 / 0xF5) which use `GC_ALLOC` (YG / nursery).
- Used for objects that must survive minor/major collections (constants, fixed singletons, jump tables).
- Reads field count from `sp[0]`, page-aligns the request, fills size and VMT into the object header.
- Clobbers `index` (used as the scratch counter for size scaling).
