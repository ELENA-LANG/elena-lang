# 0x8F -- create

- **Category:** DoubleOp
- **Enum:** `ByteCode::CreateR`
- **Operand(s):** `create {prefix}:r` (R-command, class reference)
- **Reads:** `sp[0]` (count cell pointer), `r` (class reference)
- **Writes:** `acc` (new instance), GC heap
- **Side effects:** Calls `%GC_ALLOC`; clobbers scratch registers; `index` undefined on exit.

## Semantics
Allocate an object of class `r`. The cell count is read from `[sp[0]]`, rounded up to page granularity, then passed to `%GC_ALLOC`. After the call, `acc` points at the new instance; the size header and VMT slot are written before returning.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %08Fh`

**Selection rule:** `retrieveCode(scope)` on `arg1` (`elenasrc3/engine/jitcompiler.cpp:1338`) picks the variant slot by the operand's `mskXxx` reference mask (typically `mskVMTRef` for class references). See README "Linker layout details" -- Reference masks table.

```asm
  mov  rax, [r10]
  mov  ecx, page_ceil
  shl  eax, 3
  add  ecx, eax
  and  ecx, page_mask
  call %GC_ALLOC

  mov  rcx, r10
  mov  rax, __ptr64_1
  mov  ecx, dword ptr [rcx]
  shl  ecx, 3

  mov  [rbx - elSizeOffset], rcx
  mov  [rbx - elVMTOffset], rax
```

Cell size is 8 bytes. `%GC_ALLOC` returns the new instance in `rbx`.

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %08Fh`

```asm
  mov  eax, [esi]
  mov  ecx, page_ceil
  shl  eax, 2
  add  ecx, eax
  and  ecx, page_mask
  call %GC_ALLOC

  mov  ecx, [esi]
  shl  ecx, 2
  mov  eax, __ptr32_1
  mov  [ebx - elVMTOffset], eax
  mov  [ebx - elSizeOffset], ecx
```

Cell size is 4 bytes; cached `sp[0]` is in `esi`.

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %08Fh`

```asm
  ldr     w19, [x0]
  lsl     x19, x19, #3

  add     x19, x19, page_ceil
  and     x11, x19, page_mask

#if _MAC
  adrp    x17, code_page : %GC_ALLOC
  add     x17, x17, code_pageoff : %GC_ALLOC
#elif (_LNX || _FREEBSD)
  movz    x17,  code_ptr32lo : %GC_ALLOC
  movk    x17,  code_ptr32hi : %GC_ALLOC, lsl #16
#endif
  blr     x17

  ldr     w19, [x0]
  lsl     x18, x19, #3

#if _MAC
  adrp    x19, __ptr32page_1
  add     x19, x19, __ptr32pageoff_1
#elif (_LNX || _FREEBSD)
  movz    x19,  __ptr32lo_1
  movk    x19,  __ptr32hi_1, lsl #16
#endif
  sub     x20, x10, elVMTOffset
  str     x19, [x20]
  str     w18, [x20, #12]!
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %08Fh`

```asm
  ld      r12, 0(r3)
  sldi    r12, r12, 3
  addi    r12, r12, page_ceil
  andi.   r18, r12, page_mask

  ld      r12, toc_alloc(r2)
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

`%GC_ALLOC` is invoked through the TOC slot `toc_alloc`; the VMT pointer is resolved relative to `toc_rdata`.

## Notes
- Calls `%GC_ALLOC`, so clobbers scratch registers and may trigger a GC; safepoints apply.
- Cell count is read from `[sp[0]]`, scaled by slot size (8 on 64-bit, 4 on x32), then page-rounded.
- After the call, both the size header (`-elSizeOffset`) and VMT pointer (`-elVMTOffset`) are written.
- `index` is undefined on exit; `acc` holds the new instance.
- VMT reference is patched at link time by the same mask scheme as `setr` / `peekr`.
