# 0x07 -- len

- **Category:** SingleOp
- **Enum:** `ByteCode::Len`
- **Operand(s):** (none)
- **Reads:** `[acc - elSizeOffset]` (raw object size word)
- **Writes:** `index` (element count)

## Semantics
Computes the logical length of the object in `acc`. The size header is masked with `struct_mask_inv` (clears struct/tag flags) and then divided by the element stride (8 on 64-bit, 4 on 32-bit, matching pointer width).

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %7`

```asm
  mov  edx, struct_mask_inv
  mov  rcx, [rbx-elSizeOffset]
  and  rdx, rcx
  shr  edx, 3
```

`>> 3` converts the byte size to the 8-byte slot count.

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %7`

```asm
  mov  edx, struct_mask_inv
  mov  ecx, [ebx-elSizeOffset]
  and  edx, ecx
  shr  edx, 2
```

`>> 2` because 32-bit slots are 4 bytes wide.

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %7`

```asm
  sub     x11, x10, elSizeOffset
  ldr     x9, [x11]
  and     x9, x9, struct_mask_inv
  lsr     x9, x9, #3
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %7`

```asm
  li      r16, struct_mask_inv_lo
  addis   r16, r16, struct_mask_inv_hi

  ld      r14, -elSizeOffset(r15)
  and     r14, r14, r16
  srdi    r14, r14, 3
```

`page_size_order` on this target is 3 (8-byte slots); the literal `3` is used here.

## Notes
- Reads the raw size word at `[acc - elSizeOffset]`, masks `struct_mask_inv` (clears struct/tag flags), then shifts to element count: `>> 3` on 64-bit (8-byte slot), `>> 2` on x32 (4-byte slot).
- Reports element count, NOT byte size. For raw byte length use the unshifted size header directly.
- Preserves `acc`, `sp[0]`, `sp[1]`. Clobbers `index` (and a scratch register on x86 -- `ecx`/`rcx`).
