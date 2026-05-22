# 0xE5 -- nsave dp

- **Category:** TripleOp
- **Enum:** `ByteCode::NSaveDPN`
- **Operand(s):** `nsave dp:disp, i` (immediate 32-bit `i`)

## Semantics
`dp[disp] := i` -- write the immediate constant into a frame slot.

## JIT (amd64)
**Selection rule:** `retrieveICode` selects between the small-immediate body and the large-immediate (two-instruction) body where applicable.

**Template:** `asm/amd64/core60.asm` `inline %0E5h`
```asm
mov  eax, __n_2
mov  dword ptr [rbp+__arg32_1], eax
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0E5h`
```asm
mov  eax, __n_2
mov  [ebp+__arg32_1], eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0E5h`
```asm
; small immediate (n fits in 16 bits, sign-extended)
add     x19, x29, __arg12_1
mov     x18, __n16_2
sxth    x18, x18
str     w18, [x19]
```

### Variants
| Prefix | Form |
|---|---|
| `%0E5h` | small n (16-bit, sign-extended) |
| `%01E5h` | large n (`movz` low + `movk` high << 16) |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0E5h`
```asm
; small immediate
addi    r19, r31, __arg16_1
li      r17, __n16_2
stw     r17, 0(r19)
```

### Variants
| Prefix | Form |
|---|---|
| `%0E5h` | small n (16-bit `li`) |
| `%01E5h` | large n (`lis` high + `addi` low) |

## Notes
- Stores a 32-bit immediate `i` into the data-frame slot at `dp[disp]`.
- aarch64/ppc64le carry a small/large immediate split so that a 16-bit value uses a single instruction and a wider value emits the two-instruction pair.
- On amd64/x32 the immediate is materialised in `eax` first, then stored -- there is no direct mem-imm form because the encoded slot offset already needs a `[rbp+disp]` displacement.
- No GC barrier -- `dp[disp]` is a managed/raw frame slot, not a heap pointer.
