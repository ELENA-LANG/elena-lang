# 0xA6 -- assign i

- **Category:** DoubleOp
- **Enum:** `ByteCode::AssignI`
- **Operand(s):** `assigni i`
- **Reads:** `acc`, `sp[0]`, GC table (`gc_start`, `gc_header`)
- **Writes:** `acc[i]`, card-table byte
- **Side effects:** **Write barrier** -- marks the card byte for the page containing `&acc[i]`.

## Semantics
`acc[i] := sp[0]` with a write barrier. After the field store, the GC card-table byte indexed by `(acc - gc_start) >> page_size_order` is set to 1, so the next GC knows the container object's reference set changed. Use `assigni` (rather than the barrier-less `xassigni` 0x83) whenever the stored value may be a young pointer kept alive only through an older container.

## JIT (amd64)

**Selection rule:** dispatched through the indexed-store path: `retrieveCode(arg1)` picks the inline template, and the operand slot is patched with the byte displacement of the field. Field-index encoding (the bytecode operand is a byte offset already scaled by the writer) determines whether a short-form `inline %nA6h` variant is selected on architectures that have multiple sizes for the indexed `mov`.

**Template:** `asm/amd64/core60.asm` `inline %0A6h`

```asm
  mov  rax, rbx

  ; calculate write-barrier address
  mov  rcx, [data : %CORE_GC_TABLE + gc_header]
  sub  rax, [data : %CORE_GC_TABLE + gc_start]
  shr  rax, page_size_order
  mov  [rbx + __arg32_1], r10
  mov  byte ptr [rax + rcx], 1
```

`sp[0]` is read from the cached slot `r10`.

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0A6h`

```asm
  mov  eax, ebx
  mov  [ebx + __arg32_1], esi
  ; calculate write-barrier address
  sub  eax, [data : %CORE_GC_TABLE + gc_start]
  mov  ecx, [data : %CORE_GC_TABLE + gc_header]
  shr  eax, page_size_order
  mov  byte ptr [eax + ecx], 1
```

`sp[0]` is the cached slot `esi`.

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0A6h`

```asm
  add     x11, x10, __arg12_1

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

`sp[0]` is cached in `x0`. The GC-table base is materialised with `adrp/add` (macOS) or `movz/movk` (Linux/FreeBSD).

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0A6h`

```asm
  addi    r16, r15, __arg16_1

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

GC-table base loaded via TOC (`r2`); `sp[0]` is `r3`.

## Notes

- **Has a write barrier**: card-table byte for the page containing `&acc[i]` is set to 1 so the next minor GC re-scans the container.
- For a barrier-free store (PERM/static targets, immutable containers), use `xassign i` (0x83).
- `page_size_order` is fixed by the runtime at link time (typically 12 -> 4 KiB cards). It is materialised as a constant inside the inline template.
- `sp[0]` is read from the cached register (`r10`/`esi`/`x0`/`r3`); `acc` is preserved.
