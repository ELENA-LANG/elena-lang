# 0xC0 -- cmpr

- **Category:** DoubleOp
- **Enum:** `ByteCode::CmpR`
- **Operand(s):** `cmp r` (one reference; R-cmd)
- **Reads:** `acc`
- **Writes:** `COMP.EQ`, `COMP.LT` (signed)

## Semantics
`COMP.EQ := (acc == r)`; `COMP.LT := (acc < r)` -- signed pointer compare of the accumulator against a relocatable reference operand. The argument is patched at JIT time.

## JIT (amd64)
**Selection rule:** R-command dispatched through `loadROp` (`jitcompiler.cpp:1256`). The variant prefix is chosen by the `mskXxx` reference kind of the operand (generic relocatable, nil-literal, or 32-bit signed immediate) -- not by an explicit n value.

**Template:** load `r` into a scratch GPR, compare against `acc (rbx)`.

```asm
mov  rax, __ptr64_1
cmp  rbx, rax
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0C0h` | generic `cmp r` | `mov rax, __ptr64_1` + `cmp rbx, rax` |
| `%1C0h` | `r == 0` (nil) | `test rbx, rbx` |
| `%9C0h` | 32-bit signed immediate | `mov eax, __arg32_1; movsxd rax, eax; cmp rbx, rax` |

## JIT (x32)
**Template:** direct compare against the relocated reference (x86-32 supports `cmp r32, imm32`).

```asm
cmp  ebx, __ptr32_1
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0C0h` | generic `cmp r` | `cmp ebx, __ptr32_1` |
| `%1C0h` | `r == 0` (nil) | `test ebx, ebx` |
| `%9C0h` | 32-bit signed immediate | `cmp ebx, __arg32_1` |

## JIT (aarch64)
**Template:** materialise the reference in a scratch (x11), `cmp` against `acc (x10)`.

```asm
; Linux/FreeBSD
  movz    x11, __ptr32lo_1
  movk    x11, __ptr32hi_1, lsl #16
  cmp     x10, x11
; macOS uses adrp/add page-relative addressing instead
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0C0h` | generic `cmp r` | materialise ref into `x11`, `cmp x10, x11` |
| `%1C0h` | `r == 0` (nil) | `mov x11, #0; cmp x10, x11` |
| `%9C0h` | 32-bit signed immediate | `movz/movk` + `sxtw w11` + `cmp x10, x11` |

## JIT (ppc64le)
**Template:** build the reference from the TOC + 32-bit displacement and `cmpd` against `acc (r15)`.

```asm
ld      r16, toc_code(r2)
addis   r16, r16, __xdisp32hi_1
addi    r16, r16, __xdisp32lo_1
cmpd    r15, r16
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0C0h` | generic `cmp r` | TOC-relative load into `r16`, `cmpd r15, r16` |
| `%1C0h` | `r == 0` | `li r16, 0; cmpd r15, r16` |
| `%9C0h` | `r == -1` | `li r16, 0; addi r16, r16, -1; cmpd r15, r16` |

## Notes
- COMP.EQ set by pointer-width compare; followed by a conditional jump (`jeq`, `jne`, `jlt`, ...).
- Variant `%1C0h` is a peephole optimisation for nil comparison; the JIT picks the prefix from the operand's reference mask (`mskXxx`).
- Preserves `index`; only `COMP.EQ`/`COMP.LT` are clobbered.
- Useful for instanceof tests against VMT refs and identity checks against well-known singletons.
