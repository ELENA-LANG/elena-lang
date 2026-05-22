# 0xED -- jump mssg

- **Category:** TripleOp
- **Enum:** `ByteCode::JumpMR`
- **Operand(s):** `jump mssg:m, r` (M+R)

## Semantics
Direct jump to the method resolved by the JIT from `(m, r)`. Unlike `vjump`, the target is patched in at JIT time -- no VMT lookup at runtime.

## JIT (amd64)
**Selection rule:** M+R command -- direct (non-virtual) variant. The JIT resolves `(msg, vmt)` at link time and patches the final target address into a single relative jump. No VMT slot lookup at runtime.

**Template:** `asm/amd64/core60.asm` `inline %0EDh`
```asm
jmp __relptr32_2
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0EDh`
```asm
jmp __relptr32_2
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0EDh`
```asm
#if _MAC
  adrp    x17, __ptr32page_2
  add     x17, x17, __ptr32pageoff_2
#elif (_LNX || _FREEBSD)
  movz    x17, __ptr32lo_2
  movk    x17, __ptr32hi_2, lsl #16
#endif
  br      x17
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0EDh`
```asm
ld       r12, toc_code(r2)
addis    r12, r12, __disp32hi_2
addi     r12, r12, __disp32lo_2
mtctr    r12
bctr
```

## Notes
- Direct tail-call -- the link step resolves `(msg, vmt)` and writes a final absolute/relative target.
- Contrast with `vjump mssg` (0xEC) which always indirects through the live VMT slot.
- No return path; used at the end of a method when the result of the resolved call is also the result of the caller.
- amd64/x32 emit a single `jmp rel32`; aarch64/ppc64le materialise the target then `br`/`bctr`.
