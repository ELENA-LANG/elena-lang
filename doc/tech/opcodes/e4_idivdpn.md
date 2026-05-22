# 0xE4 -- idiv dp

- **Category:** TripleOp
- **Enum:** `ByteCode::IDivDPN`
- **Operand(s):** `idiv dp:disp, n` (n = 1, 2, 4, 8)

## Semantics
`temp := n-byte:[sp[0]]; dp[disp] /= temp` -- signed integer in-place divide. Quotient written back; remainder discarded.

## JIT (amd64)
**Selection rule:** `retrieveICode(n)` picks the width: n=1 -> `%1E4h`, n=2 -> `%2E4h`, n=4 -> `%0E4h` (default), n=8 -> `%4E4h`.

**Template:** `asm/amd64/core60.asm` `inline %0E4h`
```asm
; generic / n=4 (%0E4h)
mov  rcx, [r10]
mov  rax, [rbp+__arg32_1]
cdq
idiv ecx
mov  dword ptr [rbp+__arg32_1], eax
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%0E4h` | 4 | `cdq; idiv ecx; mov dword [rbp+disp], eax` |
| `%1E4h` | 1 | `and eax,0FFh; cdq; idiv cl; mov byte [rbp+disp], al` |
| `%2E4h` | 2 | `and eax,0FFFFh; cdq; idiv cx; mov word [rbp+disp], ax` |
| `%4E4h` | 8 | `xor rdx,rdx; idiv rcx; mov [rbp+disp], rax` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0E4h`
```asm
; generic / n=4 (%0E4h)
mov  eax, [ebp+__arg32_1]
cdq
idiv dword ptr [esi]
mov  [ebp+__arg32_1], eax
```

n=8 is a full software implementation of 64-bit signed division (~60 instructions) with sign normalization, shift-based reduction and quotient correction.

### Variants
| Prefix | n | Body |
|---|---|---|
| `%0E4h` | 4 | `cdq; idiv [esi]; mov [ebp+disp], eax` |
| `%1E4h` | 1 | `cdq; idiv cl; mov byte ptr [ebp+disp], al` |
| `%2E4h` | 2 | `cdq; idiv cx; mov word ptr [ebp+disp], ax` |
| `%4E4h` | 8 | software 64-bit signed div (sign-fix + shift loop) |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0E4h`
```asm
; generic / n=4 (%0E4h)
add     x19, x29, __arg12_1
ldrsw   x17, [x0]
ldrsw   x18, [x19]
sdiv    x18, x18, x17
str     w18, [x19]
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%0E4h` | 4 | `ldrsw`/`sdiv`/`str w` |
| `%1E4h` | 1 | `ldrsb`/`sdiv`/`strb w` |
| `%2E4h` | 2 | `ldrsb`/`sdiv`/`strh w` |
| `%4E4h` | 8 | `ldr`/`sdiv`/`str x` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0E4h`
```asm
; generic / n=4 (%0E4h)
addi    r19, r31, __arg16_1
lwz     r17, 0(r3)
lwz     r18, 0(r19)
divw    r18, r18, r17
stw     r18, 0(r19)
```

### Variants
| Prefix | n | Divide opcode |
|---|---|---|
| `%0E4h` | 4 | `divw` |
| `%1E4h` | 1 | `divw` (byte load/store) |
| `%2E4h` | 2 | `divw` (half load/store) |
| `%4E4h` | 8 | `divd` |

## Notes
- Signed integer divide; quotient stored back into `dp[disp]`, remainder is dropped.
- On x32 the n=8 variant is a full software 64-bit signed division (~60 instructions: sign-normalisation, shift loop, quotient correction).
- aarch64 uses `sdiv`; ppc64le uses `divw`/`divd` -- no `cdq` sign extension is needed there.
- Divide-by-zero is not trapped/guarded in the inline body; that is the caller's responsibility.
