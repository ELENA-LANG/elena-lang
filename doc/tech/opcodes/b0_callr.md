# 0xB0 -- call (R-cmd)

- **Category:** DoubleOp (R-cmd)
- **Enum:** `ByteCode::CallR`
- **Operand(s):** `call {prefix}:r`
- **Reads:** none directly -- the target is a relocatable reference
- **Writes:** `pc`, return-address slot on stack; on return `acc` holds the result
- **Side effects:** Callee may clobber any caller-saved register; the `sp[0]`/`sp[1]` cache is undefined across the call (use `xflush`/`xrefresh` to preserve).

## Semantics
Call the procedure at the relocatable reference `r`. The R-cmd encoding tags the operand with a *prefix* (`symbol:`, `mssg:`, `extern:`, `export:`, `lazy:`, ...) that selects the relocation kind; the emitted body is always a single direct call. See `loadCallROp` in `jitcompiler.cpp` for the accepted prefix -> relocation mapping. Compare with `xcall` (0x2F, indirect through `acc`) and `callvi` (0xB1, VMT-indirect).

### Reference kinds (prefix table)
| Prefix in `.esm` | Reference kind | Notes |
|---|---|---|
| `symbol:r` | code symbol | direct symbol call (common case) |
| `mssg:m, r` | resolved method | JIT picks implementation by `(class r, message m)` |
| `extern:r` | imported procedure | resolved through the platform import table |
| `export:r` | exported symbol | inverse of import; ELENA -> host bridge |
| `lazy:r` | lazy-resolved | patched on first dispatch |

## JIT (amd64)

**Selection rule:** R-cmd dispatched through `loadCallROp` -- the variant is selected by the `mskXxx` of the reference: `mskSymbolRef` (ELENA procedures, the dominant case) emits a single `call rel32`; `mskExternalRef` routes through a platform import thunk; `mskMessage` resolves the receiver class at JIT time and rewrites to a direct symbol call. The `r` operand is patched with the relocation kind picked by the prefix.

**Template:** `asm/amd64/core60.asm` `inline %0B0h`

```asm
  call __relptr32_1
```

`call rel32` with a 32-bit relative slot patched by the JIT.

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0B0h`

```asm
  call __relptr32_1
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0B0h`

```asm
#if _MAC
  adrp    x17, __ptr32page_1
  add     x17, x17, __ptr32pageoff_1
#elif (_LNX || _FREEBSD)
  movz    x17,  __ptr32lo_1
  movk    x17,  __ptr32hi_1, lsl #16
#endif
  blr     x17
```

aarch64 has no PC-relative call to an arbitrary 32-bit address: the target is materialised in `x17` (`adrp/add` on macOS, `movz/movk` on Linux/FreeBSD) and `blr` branches to it.

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0B0h`

```asm
  ld       r12, toc_code(r2)
  addis    r12, r12, __disp32hi_1
  addi     r12, r12, __disp32lo_1
  mtctr    r12            ; put code address into ctr
  bctrl                   ; and call it
```

ppc64le ABI: load TOC base, add high/low 16-bit halves of the displacement, copy into CTR, then `bctrl` (branch-to-CTR-and-link).

## Notes

- **Not** a tail call -- control returns here, then a `quit` (0x04) returns from the caller. Use `xjump` (0x07) family for tail dispatch.
- Preserves nothing: the callee may clobber any caller-saved register. On return, `acc` holds the result.
- The `sp[0]`/`sp[1]` cache is undefined across the call -- bracket with `xflush`/`xrefresh` if you need to preserve top-of-stack values past the call.
- aarch64 has no PC-relative call to an arbitrary 32-bit address; the target is materialised in `x17` before `blr`.
- ppc64le routes through the TOC; the callee may shuffle `r2` for cross-module calls (ABI conventional prologue).
