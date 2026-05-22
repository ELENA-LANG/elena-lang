# 0xE8 -- nadd dp

- **Category:** TripleOp
- **Enum:** `ByteCode::NAddDPN`
- **Operand(s):** `nadd dp:disp, n` (immediate `n`)

## Semantics
`dp[disp] += n` -- in-place add of an immediate constant.

## JIT (amd64)
**Selection rule:** `retrieveICode` chooses the immediate width -- small positive `n` uses the single-instruction body; aarch64/ppc64le also have a "small negative" variant (emits `sub`) and a "large `n`" variant (`movz` + `movk` / `lis` + `addi`).

**Template:** `asm/amd64/core60.asm` `inline %0E8h`
```asm
mov  eax, __n_2
add  dword ptr [rbp+__arg32_1], eax
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0E8h`
```asm
mov  eax, __n_2
add  [ebp+__arg32_1], eax
```

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0E8h`
```asm
; small positive n (%0E8h)
add     x19, x29, __arg12_1
mov     x18, __n16_2
ldrsw   x20, [x19]

add     x20, x20, x18
str     w20, [x19]
```

### Variants
| Prefix | Form |
|---|---|
| `%0E8h` | small positive n (16-bit) |
| `%8E8h` | small negative n (uses `sub`) |
| `%09E8h` | large n (`movz` + `movk`) -- positive |
| `%0AE8h` | large n (`movz` + `movk`) -- variant |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0E8h`
```asm
addi    r19, r31, __arg16_1
li      r17, __n16_2
lwz     r18, 0(r19)
add     r18, r18, r17
stw     r18, 0(r19)
```

## Notes
- In-place add of an immediate; the immediate width determines the picked variant.
- aarch64 emits `sub` instead of `add` for negative `n` (the `%8E8h` slot).
- No overflow trap; pure 32-bit wraparound (or 64-bit on the wide variants).
- Used heavily by compiled loop counters and pointer adjustments.
