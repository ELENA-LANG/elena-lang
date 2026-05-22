# 0xE1 -- iadd dp

- **Category:** TripleOp
- **Enum:** `ByteCode::IAddDPN`
- **Operand(s):** `iadd dp:disp, n` (n = 1, 2, 4, 8)

## Semantics
`temp := n-byte:[sp[0]]; dp[disp] += temp` -- integer in-place add, sized by `n`.

## JIT (amd64)
**Selection rule:** `retrieveICode(n)` picks the width-specific variant: n=1 -> `%1E1h`, n=2 -> `%2E1h`, n=4 -> `%0E1h` (default), n=8 -> `%4E1h`. Other widths are invalid.

**Template:** `asm/amd64/core60.asm` `inline %0E1h`
```asm
; generic / n=4 (%0E1h)
lea  rdi, [rbp + __arg32_1]
mov  rax, [r10]
add  dword ptr [rdi], eax
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%0E1h` | 4 | `add dword ptr [rdi], eax` |
| `%1E1h` | 1 | `add byte ptr [rdi], al` |
| `%2E1h` | 2 | `add word ptr [rdi], ax` |
| `%4E1h` | 8 | `add [rdi], rax` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0E1h`
```asm
; generic / n=4 (%0E1h)
lea  edi, [ebp + __arg32_1]
mov  eax, [esi]
add  [edi], eax
```

n=8 emits a 64-bit add via dual 32-bit `add`/`adc`.

### Variants
| Prefix | n | Body |
|---|---|---|
| `%0E1h` | 4 | `add [edi], eax` |
| `%1E1h` | 1 | `add byte ptr [edi], al` |
| `%2E1h` | 2 | `add word ptr [edi], ax` |
| `%4E1h` | 8 | `mov eax,[esi+4]; mov ecx,[esi]; add [edi],ecx; adc [edi+4],eax` |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0E1h`
```asm
; generic / n=4 (%0E1h)
add     x19, x29, __arg12_1
ldrsw   x17, [x0]
ldrsw   x18, [x19]
add     x17, x17, x18
str     w17, [x19]
```

### Variants
| Prefix | n | Load/store width |
|---|---|---|
| `%0E1h` | 4 | `ldrsw`/`str w` |
| `%1E1h` | 1 | `ldrsb`/`strb w` |
| `%2E1h` | 2 | `ldrsh`/`strh w` |
| `%4E1h` | 8 | `ldr`/`str x` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0E1h`
```asm
; generic / n=4 (%0E1h)
addi    r19, r31, __arg16_1
lwz     r17, 0(r3)
lwz     r18, 0(r19)
add     r17, r17, r18
stw     r17, 0(r19)
```

### Variants
| Prefix | n | Load/store width |
|---|---|---|
| `%0E1h` | 4 | `lwz`/`stw` |
| `%1E1h` | 1 | `lbz`/`stb` |
| `%2E1h` | 2 | `lhz`/`sth` |
| `%4E1h` | 8 | `ld`/`std` |

## Notes
- Width-driven variants per `n`  in  {1,2,4,8}.
- On x32 the n=8 variant expands into dual 32-bit `add`/`adc` to carry across the 64-bit value.
- No overflow trap -- wraparound semantics.
- Source operand is `sp[0]`; destination is `dp[disp]` (in-place RMW).
