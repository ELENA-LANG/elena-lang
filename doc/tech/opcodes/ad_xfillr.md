# 0xAD -- xfill (R-cmd)

- **Category:** DoubleOp (R-cmd)
- **Enum:** `ByteCode::XFillR`
- **Operand(s):** `xfill r`
- **Reads:** `acc` (destination base), `sp[0]` (boxed int count; first word holds count)
- **Writes:** `[acc .. acc + count*ptr]`
- **Side effects:** Scratch on platform fill registers / EFLAGS.

## Semantics
Fill the buffer pointed to by `acc` with the reference value `r`, repeated `int(sp[0])` times. The repeat count is the first machine word of the object pointed to by `sp[0]` (standard boxed-int shape). Compare with `fill i,r` (0x82-family) which uses an immediate count.

## JIT (amd64)

**Selection rule:** R-cmd dispatched through `loadROp` -- the variant is selected by the *kind* of the reference (`mskXxx`): a fresh `mskSymbolRef` / `mskRDataRef` slot gets the full template that materialises the reference into the fill register, while a nil/`0` reference picks the `%1ADh` variant that zeros the register and skips relocation. The `r` operand is rewritten according to `mskXxx`: 32-bit absolute on x32/amd64, page+offset (`adrp/add`) on aarch64 macOS, hi/lo halves (`movz/movk`) on Linux/FreeBSD, TOC-relative (`addis/addi`) on ppc64le.

**Template:** `asm/amd64/core60.asm` `inline % 0ADh`

```asm
  mov  rcx, r10
  mov  rax, __ptr64_1
  mov  ecx, dword ptr [rcx]
  mov  rdi, rbx
  rep  stos
```

### Variants
| Prefix | When | Body |
|---|---|---|
| `%0ADh` | generic | load 64-bit reference into `rax`, `rep stosq` (8-byte units) |
| `%1ADh` | `r = 0` / nil | `xor rax, rax` (no relocation slot) |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline % 0ADh`

```asm
  mov  eax, __ptr32_1
  mov  edi, ebx
  mov  ecx, [esi]
  rep  stos
```

### Variants
| Prefix | When | Body |
|---|---|---|
| `%0ADh` | generic | load 32-bit reference into `eax`, `rep stosd` (4-byte units) |
| `%1ADh` | `r = 0` / nil | `xor eax, eax` |

Count is loaded from `[esi]` (x32 `sp[0]` cache).

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline % 0ADh`

```asm
  ldr     w11, [x0]
  lsl     x11, x11, #3

#if _MAC
  adrp    x12, __ptr32page_1
  add     x12, x12, __ptr32pageoff_1
#elif (_LNX || _FREEBSD)
  movz    x12,  __ptr32lo_1
  movk    x12,  __ptr32hi_1, lsl #16
#endif
  mov     x13, x10

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 8
  str     x12, [x13], #8
  b       labLoop

labEnd:
```

### Variants
| Prefix | When | Body |
|---|---|---|
| `%0ADh` | generic | materialise `r` into `x12` (`adrp/add` or `movz/movk`), open-coded `str`-loop |
| `%1ADh` | `r = 0` / nil | `movz x12, #0` (no relocation) |

`x0 = sp[0]` (boxed int); the count is loaded with `ldr w11` then scaled `<<3` to bytes.

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline % 0ADh`

```asm
  ld      r16, 0(r3)
  sldi    r16, r16, 3

  ld      r17, toc_code(r2)
  addis   r17, r17, __xdisp32hi_1
  addi    r17, r17, __xdisp32lo_1

  mr      r18, r15

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -8
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:
```

### Variants
| Prefix | When | Body |
|---|---|---|
| `%0ADh` | generic | resolve `r` via TOC + `addis/addi`, open-coded `std`-loop |
| `%1ADh` | `r = 0` / nil | `li r17, 0` (no relocation) |

`r3 = sp[0]`; count is loaded as a word from `[r3]` then scaled `<<3`.

## Notes

- Fills `count` consecutive pointer-sized words starting at `acc` with the reference value `r` (typically `nil`, a class pointer, or a constant).
- Count comes from `int(sp[0])` -- the *first machine word* of the object referenced by `sp[0]` (standard boxed-int unboxing).
- Compare with the immediate-count variant `fill i,r` (0x82 family).
- Scratches: amd64/x32 use `rep stos` (clobbers `rcx`/`ecx`, `rdi`/`edi`, EFLAGS); aarch64/ppc64le use an open-coded loop.
- The `%1ADh` variant exists purely as a peephole optimisation for the common `xfill r:0`/nil-fill pattern -- no relocation slot is reserved.
