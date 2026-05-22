# 0xE3 -- imul dp

- **Category:** TripleOp
- **Enum:** `ByteCode::IMulDPN`
- **Operand(s):** `imul dp:disp, n` (n = 1, 2, 4, 8)

## Semantics
`temp := n-byte:[sp[0]]; dp[disp] *= temp` -- signed integer in-place multiply.

## JIT (amd64)
**Selection rule:** `retrieveICode(n)` picks the width: n=1 -> `%1E3h`, n=2 -> `%2E3h`, n=4 -> `%0E3h` (default), n=8 -> `%4E3h`.

**Template:** `asm/amd64/core60.asm` `inline %0E3h`
```asm
; generic / n=4 (%0E3h)
mov  rcx, [r10]
mov  rax, [rbp+__arg32_1]
imul ecx
mov  dword ptr [rbp+__arg32_1], eax
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%0E3h` | 4 | `imul ecx; mov dword [rbp+disp], eax` |
| `%1E3h` | 1 | `imul cl; mov byte [rbp+disp], al` |
| `%2E3h` | 2 | `imul cx; mov word [rbp+disp], ax` |
| `%4E3h` | 8 | `imul rcx; mov [rbp+disp], rax` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0E3h`
```asm
; generic / n=4 (%0E3h)
mov  eax, [ebp+__arg32_1]
imul dword ptr [esi]
mov  [ebp+__arg32_1], eax
```

n=8 implements 64x64 multiplication via three 32x32 `mul` operations with `ebx` spill.

### Variants
| Prefix | n | Body |
|---|---|---|
| `%0E3h` | 4 | `imul [esi]; mov [ebp+disp], eax` |
| `%1E3h` | 1 | `imul cl; mov byte ptr [ebp+disp], al` |
| `%2E3h` | 2 | `imul cx; mov word ptr [ebp+disp], ax` |
| `%4E3h` | 8 | manual 64-bit mul: `SHI*DLO + SLO*DHI + SLO*DLO` |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0E3h`
```asm
; generic / n=4 (%0E3h)
add     x19, x29, __arg12_1
ldrsw   x17, [x0]
ldrsw   x18, [x19]
mul     x17, x17, x18
str     w17, [x19]
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%0E3h` | 4 | `ldrsw`/`mul`/`str w` |
| `%1E3h` | 1 | `ldrsb`/`mul`/`strb w` |
| `%2E3h` | 2 | `ldrsb`/`mul`/`strh w` |
| `%4E3h` | 8 | `ldr`/`mul`/`str x` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0E3h`
```asm
; generic / n=4 (%0E3h)
addi    r19, r31, __arg16_1
lwz     r17, 0(r3)
lwz     r18, 0(r19)
mullw   r17, r17, r18
stw     r17, 0(r19)
```

### Variants
| Prefix | n | Multiply opcode |
|---|---|---|
| `%0E3h` | 4 | `mullw` |
| `%1E3h` | 1 | `mullw` (byte load/store) |
| `%2E3h` | 2 | `mullw` (half load/store) |
| `%4E3h` | 8 | `mulld` (64-bit) |

## Notes
- Width-driven variants per `n`  in  {1,2,4,8}.
- Signed integer multiply; the low half (width-clamped) is stored back.
- On x32 the n=8 variant synthesises a 64x64 multiply from three 32x32 `mul` ops (SHI*DLO + SLO*DHI + SLO*DLO) with `ebx` spilled.
- No overflow check.
