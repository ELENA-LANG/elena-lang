# 0x80 -- setr

- **Category:** DoubleOp
- **Enum:** `ByteCode::SetR`
- **Operand(s):** `set {prefix}:r` (R-command, reference operand)
- **Reads:** reference table
- **Writes:** `acc`

## Semantics
`acc := r` -- load a reference into the accumulator. The actual relocation kind (data, rdata, mssg, code, stat, mdata, ...) is selected via the prefix; the JIT patches the immediate at link time. Three variant slots cover the normal pointer, the null reference (arg == 0) and a small signed-integer ref (arg < 0).

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %80h`

**Selection rule:** The JIT calls `retrieveCode(scope)` on `arg1` (see `elenasrc3/engine/jitcompiler.cpp:1338`) and chooses the variant by the operand's `mskXxx` reference mask -- each variant slot is the addressing-mode body for one section kind (code, rdata, data, stat, mdata, extern). See README "Linker layout details" -- Reference masks table. Additionally, the null (`arg == 0`) and small-signed-constant (`arg < 0`) cases get dedicated short forms.

```asm
  mov  rbx, __ptr64_1
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%80h`  | normal positive ref id | `mov rbx, __ptr64_1` (relocated 64-bit pointer) |
| `%180h` | arg == 0 (null ref) | `xor rbx, rbx` |
| `%980h` | arg < 0 (small signed constant) | `mov ebx, __arg32_1` / `movsxd rbx, ebx` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %80h`

```asm
  mov  ebx, __ptr32_1
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%80h`  | normal positive ref id | `mov ebx, __ptr32_1` |
| `%180h` | arg == 0 (null ref) | `xor ebx, ebx` |
| `%980h` | arg < 0 (small signed constant) | `mov ebx, __arg32_1` |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %80h`

```asm
#if _MAC
  adrp    x10, __ptr32page_1
  add     x10, x10, __ptr32pageoff_1
#elif (_LNX || _FREEBSD)
  movz    x10,  __ptr32lo_1
  movk    x10,  __ptr32hi_1, lsl #16
#endif
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%80h`  | normal positive ref id | ADRP/ADD (Mach-O) or MOVZ/MOVK pair (ELF) |
| `%180h` | arg == 0 (null ref) | `mov x10, #0` |
| `%980h` | arg < 0 (small signed constant) | MOVZ/MOVK + `sxtw x10, w10` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %80h`

```asm
  ld      r15, toc_code(r2)
  addis   r15, r15, __xdisp32hi_1
  addi    r15, r15, __xdisp32lo_1
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%80h`  | normal positive ref id | TOC-relative load + `addis`/`addi` displacement pair |
| `%180h` | arg == 0 (null ref) | `li r15, 0` |
| `%980h` | arg < 0 (small signed constant) | `li r15, 0` / `addi r15, r15, -1` |

## Notes
- Variant prefix matches the operand's `mskXxx` (see [`../linker.md`](../linker.md) -- Reference masks).
- Reference is patched at link time; emitted body is just the addressing-mode skeleton.
- Preserves `index`, `sp[0]`, `sp[1]` -- only `acc` is written.
- `arg == 0` and `arg < 0` get dedicated short forms (null / small signed constant).
