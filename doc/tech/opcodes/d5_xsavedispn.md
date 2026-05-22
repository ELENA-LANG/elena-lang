# 0xD5 -- xsave disp

- **Category:** TripleOp
- **Enum:** `ByteCode::XSaveDispN`
- **Operand(s):** `xsave disp, n` (disp into `acc`, n 32-bit immediate)
- **Reads:** --
- **Writes:** `acc[disp]` (32 bits)

## Semantics
Store the 32-bit immediate `n` at offset `disp` from the accumulator object: `[acc + disp] := n`. Used to materialise compile-time constants directly into freshly-allocated objects without an extra register move.

## JIT (amd64)
**Selection rule:** dispatched through `loadDispNOp` (`jitcompiler.cpp:2585`) -- `retrieveNOpIndex(arg2, extendedForm, 0)`. **Only 2 slots:** small (|n| <= `extendedForm` -> slot 0) or extended (|n| > `extendedForm` -> slot 1). The extended form encodes the immediate `n` with a wider instruction sequence (`movz`+`movk` on aarch64, `li`+`addis` on ppc64le). Note: the selector keys on `arg2` (the value `n` being stored), not on `arg1` (the displacement).

**Template:** immediate move into a 32-bit memory location at `rbx + disp`.

```asm
mov  eax, __n_2
mov  dword ptr [rbx + __arg32_1], eax
```

## JIT (x32)
**Template:** identical (`ebx + disp`).

```asm
mov  eax, __n_2
mov  [ebx + __arg32_1], eax
```

## JIT (aarch64)
**Template:** materialise the 16-bit value (sign-extended via `sxth`), store as a 32-bit word; a second prefix handles 32-bit constants via `movz/movk`.

```asm
; %0D5h -- n fits in 16 bits
add     x19, x10, __arg12_1
mov     x18, __n16_2
sxth    x18, x18
str     w18, [x19]
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0D5h` | 16-bit n | `mov + sxth + str w` |
| `%01D5h` | 32-bit n | `movz x18, __n16_2; movk x18, __n16hi_2, lsl #16; str w18` |

## JIT (ppc64le)
**Template:** `li` the 16-bit value, `stw`. Wider constants use a second prefix with `addis`.

```asm
; %0D5h
addi    r19, r15, __arg16_1
li      r17, __n16_2
stw     r17, 0(r19)
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0D5h` | 16-bit n | `li + stw` |
| `%1D5h` | 32-bit n | `li lo; andi.; addis hi; stw` |

## Notes
- Undocumented in `bytecode60.txt`; the assembly clearly implements an "immediate store into acc[disp]".
- Closely related to `nsave dp:disp, i` (0xE5) but writes to the *object* (`acc + disp`) rather than the data-frame slot.
- Two variants: short displacement (|n| <= `extendedForm` -> 16-bit encoding) and extended (full 32-bit encoding via `movz`/`movk` or `li`/`addis`).
- Selection key is the **value n** (the immediate to store), not the displacement -- the picker chooses the encoding wide enough to fit `n`.
- 32-bit store (not pointer-wide); high bits of `acc[disp]` are unaffected.
