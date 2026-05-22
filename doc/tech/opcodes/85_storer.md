# 0x85 -- storer

- **Category:** DoubleOp
- **Enum:** `ByteCode::StoreR`
- **Operand(s):** `store {prefix}:r` (R-command, reference operand)
- **Reads:** `acc`
- **Writes:** memory at the address of `r`
- **Side effects:** Direct write -- no GC write barrier (static slots are root-scanned).

## Semantics
`[r] := acc` -- store the accumulator into the static slot `r`. Inverse of `peek r`. The prefix encodes the relocation kind which the JIT patches at link time.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %85h`

**Selection rule:** `retrieveCode(scope)` on `arg1` (`elenasrc3/engine/jitcompiler.cpp:1338`) picks the variant slot by the operand's `mskXxx` reference mask -- one body per section kind (code, rdata, data, stat, mdata, extern). See README "Linker layout details" -- Reference masks table.

```asm
  mov  rax, __ptr64_1
  mov  [rax], rbx
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %85h`

```asm
  mov  [__ptr32_1], ebx
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %85h`

```asm
#if _MAC
  adrp    x11, __ptr32page_1
  add     x11, x11, __ptr32pageoff_1
#elif (_LNX || _FREEBSD)
  movz    x11,  __ptr32lo_1
  movk    x11,  __ptr32hi_1, lsl #16
#endif
  str     x10, [x11]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %85h`

```asm
  ld      r16, toc_code(r2)
  addis   r16, r16, __xdisp32hi_1
  addi    r16, r16, __xdisp32lo_1

  std     r15, 0(r16)
```

## Notes
- Pairs with `peek r` (0x84); together they implement static-slot read/write.
- Variant prefix matches the operand's `mskXxx` (see [`../linker.md`](../linker.md) -- Reference masks).
- Direct write -- no GC write barrier (static slots are scanned as GC roots).
- Preserves `acc`, `index`, `sp[0..1]` -- only the static slot is written.
