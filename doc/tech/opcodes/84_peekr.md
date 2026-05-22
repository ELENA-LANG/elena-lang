# 0x84 -- peekr

- **Category:** DoubleOp
- **Enum:** `ByteCode::PeekR`
- **Operand(s):** `peek {prefix}:r` (R-command, reference operand)
- **Reads:** memory at the address of `r`
- **Writes:** `acc`

## Semantics
`acc := [r]` -- load the word stored at the static address `r` into the accumulator. Used to read static fields / global slots. The prefix encodes the relocation kind which the JIT patches at link time.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %84h`

**Selection rule:** `retrieveCode(scope)` on `arg1` (`elenasrc3/engine/jitcompiler.cpp:1338`) picks the variant slot by the operand's `mskXxx` reference mask -- one body per section kind (code, rdata, data, stat, mdata, extern). See README "Linker layout details" -- Reference masks table.

```asm
  mov  rax, __ptr64_1
  mov  rbx, [rax]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %84h`

```asm
  mov  ebx, [__ptr32_1]
```

x86-32 absolute addressing fits into the `mov` displacement; the load is folded into a single instruction.

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %84h`

```asm
#if _MAC
  adrp    x11, __ptr32page_1
  add     x11, x11, __ptr32pageoff_1
#elif (_LNX || _FREEBSD)
  movz    x11,  __ptr32lo_1
  movk    x11,  __ptr32hi_1, lsl #16
#endif
  ldr     x10, [x11]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %84h`

```asm
  ld      r16, toc_code(r2)
  addis   r16, r16, __xdisp32hi_1
  addi    r16, r16, __xdisp32lo_1

  ld      r15, 0(r16)
```

## Notes
- Inverse of `store r` (0x85); together they implement static-slot read/write.
- Variant prefix matches the operand's `mskXxx` (see [`../linker.md`](../linker.md) -- Reference masks).
- Reference is patched at link time.
- Preserves `index`, `sp[0..1]`; only `acc` is written.
