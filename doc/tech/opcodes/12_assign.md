# 0x12 -- assign

- **Category:** SingleOp
- **Enum:** `ByteCode::Assign`
- **Operand(s):** (none)
- **Reads:** `acc`, `index`, `sp[0]`, GC table (`gc_start`, `gc_header`)
- **Writes:** `acc[index]`, GC card byte
- **Side effects:** Emits a write barrier -- marks the card covering `acc` as dirty so the collector can scan the modified reference.

## Semantics
`acc[index] := sp[0]` with write barrier: the card byte at `gc_header + ((acc - gc_start) >> page_size_order)` is set to 1.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %12h`

```asm
  mov  rax, rbx

  ; calculate write-barrier address
  mov  rcx, [data : %CORE_GC_TABLE + gc_header]
  sub  rax, [data : %CORE_GC_TABLE + gc_start]
  shr  rax, page_size_order
  mov  [rbx + rdx*8], r10
  mov  byte ptr [rax + rcx], 1
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %12h`

```asm
  mov  eax, ebx
  mov  [ebx + edx*4], esi
  ; calculate write-barrier address
  sub  eax, [data : %CORE_GC_TABLE + gc_start]
  mov  ecx, [data : %CORE_GC_TABLE + gc_header]
  shr  eax, page_size_order
  mov  byte ptr [eax + ecx], 1
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %12h`

```asm
  lsl     x11, x9, 3
  add     x11, x11, x10

  ; calculate write-barrier address
#if _MAC
  adrp    x12, data_page : %CORE_GC_TABLE
  add     x12, x12, data_pageoff : %CORE_GC_TABLE
#elif (_LNX || _FREEBSD)
  movz    x12, data_ptr32lo : %CORE_GC_TABLE
  movk    x12, data_ptr32hi : %CORE_GC_TABLE, lsl #16
#endif

  add     x13, x12, gc_start
  add     x15, x12, gc_header
  ldr     x14, [x13]
  sub     x14, x10, x14
  ldr     x15, [x15]

  lsr     x14, x14, page_size_order
  add     x14, x15, x14
  mov     x12, 1
  strb    w12, [x14]

  str     x0, [x11]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %12h`

```asm
  sldi    r16, r14, 3
  add     r16, r16, r15

  ; calculate write-barrier address
  ld      r19, toc_gctable(r2)

  ld      r17, gc_start(r19)
  ld      r18, gc_header(r19)

  subf    r19, r17, r15
  srdi    r19, r19, page_size_order

  std     r3, 0(r16)
  li      r20, 1
  add     r18, r18, r19
  stb     r20, 0(r18)
```

## Notes
- Indexed store WITH GC write barrier: `acc[index] := sp[0]`, then mark the card covering `acc` dirty. Card index = `(acc - gc_start) >> page_size_order`.
- For barrier-free stores use `xassign` (0x19, dynamic index) or `xassign i` (0x83, immediate index). Use `assign` only when storing a managed reference into a heap object.
- Stride is `*8` on 64-bit, `*4` on x32 -- index counts slots, not bytes.
- Preserves `index`, `sp[0]`. `acc` is read but the register holding it is reused as a scratch (subtraction), so callers should not rely on `acc` being unchanged past this point on amd64/x32.
