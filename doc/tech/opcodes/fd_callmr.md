# 0xFD -- call mssg

- **Category:** TripleOp
- **Enum:** `ByteCode::CallMR`
- **Operand(s):** `call mssg:m, r` (M+R)

## Semantics
Direct call to the method resolved at JIT time from `(m, r)`. No VMT lookup at runtime -- the target address is patched into the call site.

## JIT (amd64)
**Selection rule:** M+R command -- direct (non-virtual) variant. The JIT resolves `(m, r)` to a final code address at link time and patches a single relative call.

**Template:** `asm/amd64/core60.asm` `inline %0FDh`
```asm
call __relptr32_2
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0FDh`
```asm
call __relptr32_2
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0FDh`
```asm
#if _MAC
  adrp    x17, __ptr32page_2
  add     x17, x17, __ptr32pageoff_2
#elif (_LNX || _FREEBSD)
  movz    x17, __ptr32lo_2
  movk    x17, __ptr32hi_2, lsl #16
#endif
  blr     x17
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0FDh`
```asm
ld       r12, toc_code(r2)
addis    r12, r12, __disp32hi_2
addi     r12, r12, __disp32lo_2
mtctr    r12
bctrl
```

## Notes
- Direct call -- resolved at link time, no VMT lookup at runtime.
- Contrast with `vcall mssg` (0xFC) which always goes through the live VMT slot.
- amd64/x32 emit a single `call rel32`; aarch64/ppc64le materialise the target then `blr`/`bctrl`.
- Used when the compiler can statically prove the receiver type / target method (final / sealed methods, devirtualised calls).
