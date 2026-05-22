# 0x7F -- fpi dp

- **Category:** DoubleOp
- **Enum:** `ByteCode::FPiDP`
- **Operand(s):** `disp` (frame-pointer displacement)
- **Reads:** (none)
- **Writes:** `double:dp[disp]`

## Semantics
Stores the constant pi at `dp[disp]`: `double:dp[disp] := pi`. x87 backends use the `fldpi` instruction; the aarch64 backend reads the constant from offset 168 of `CORE_MATH_TABLE`. The ppc64le backend is not implemented.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %7Fh`

```asm
  lea   rdi, [rbp + __arg32_1]
  fldpi
  fstp  qword ptr [rdi]    ; store result
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %7Fh`

```asm
  lea   edi, [ebp + __arg32_1]
  fldpi
  fstp  qword ptr [edi]    ; store result
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %7Fh`

```asm
  add     x19, x29, __arg12_1 ; dest (x19)

#if _MAC
  adrp    x20, rdata_page : %CORE_MATH_TABLE
  add     x20, x20, rdata_pageoff : %CORE_MATH_TABLE
#elif (_LNX || _FREEBSD)
  movz    x20, rdata_ptr32lo : %CORE_MATH_TABLE
  movk    x20, rdata_ptr32hi : %CORE_MATH_TABLE, lsl #16
#endif

  ldr     d0, [x20, #168] ; d1 <- PI
  str     d0, [x19]
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %7Fh`

*Not implemented in this backend.*

## Notes
- Trivial constant-load: writes the IEEE-754 representation of pi to `dp[disp]`. No FP computation involved.
- x87 backends use the dedicated `fldpi` instruction (built into the x87 ISA since the 8087).
- aarch64 reads the precomputed PI value from `%CORE_MATH_TABLE + 168` -- the same constant slot used by the trig family.
- ppc64le has **no implementation** -- `fldpi` has no POWER analogue and the table-load path has not been wired up.
- No inputs read; only the destination slot is written. `acc`, `index`, and stack are preserved.
