# 0xE2 -- isub dp

- **Category:** TripleOp
- **Enum:** `ByteCode::ISubDPN`
- **Operand(s):** `isub dp:disp, n` (n = 1, 2, 4, 8)

## Semantics
`temp := n-byte:[sp[0]]; dp[disp] -= temp` -- integer in-place subtract.

## JIT (amd64)
**Selection rule:** `retrieveICode(n)` picks the width: n=1 -> `%1E2h`, n=2 -> `%2E2h`, n=4 -> `%0E2h` (default), n=8 -> `%4E2h`.

**Template:** `asm/amd64/core60.asm` `inline %0E2h`
```asm
; generic / n=4 (%0E2h)
lea  rdi, [rbp + __arg32_1]
mov  rax, [r10]
sub  dword ptr [rdi], eax
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%0E2h` | 4 | `sub dword ptr [rdi], eax` |
| `%1E2h` | 1 | `sub byte ptr [rdi], al` |
| `%2E2h` | 2 | `sub word ptr [rdi], ax` |
| `%4E2h` | 8 | `sub [rdi], rax` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0E2h`
```asm
; generic / n=4 (%0E2h)
lea  edi, [ebp + __arg32_1]
mov  eax, [esi]
sub  [edi], eax
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%0E2h` | 4 | `sub [edi], eax` |
| `%1E2h` | 1 | `sub byte ptr [edi], al` |
| `%2E2h` | 2 | `sub word ptr [edi], ax` |
| `%4E2h` | 8 | `mov eax,[esi+4]; mov ecx,[esi]; sub [edi],ecx; sbb [edi+4],eax` |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0E2h`
```asm
; generic / n=4 (%0E2h)
add     x19, x29, __arg12_1
ldr     w17, [x0]
ldr     w18, [x19]
sub     x17, x18, x17
str     w17, [x19]
```

### Variants
| Prefix | n | Load/store width |
|---|---|---|
| `%0E2h` | 4 | `ldr w`/`str w` |
| `%1E2h` | 1 | `ldrsb`/`strb w` |
| `%2E2h` | 2 | `ldrsw`/`str w` |
| `%4E2h` | 8 | `ldr`/`str x` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0E2h`
```asm
; generic / n=4 (%0E2h)
addi    r19, r31, __arg16_1
lwz     r17, 0(r3)
lwz     r18, 0(r19)
subf    r17, r17, r18
stw     r17, 0(r19)
```

### Variants
| Prefix | n | Load/store width |
|---|---|---|
| `%0E2h` | 4 | `lwz`/`stw` |
| `%1E2h` | 1 | `lbz`/`stb` |
| `%2E2h` | 2 | `lhz`/`sth` |
| `%4E2h` | 8 | `ld`/`std` |

## Notes
- Width-driven variants per `n`  in  {1,2,4,8}.
- On x32 the n=8 variant expands into dual 32-bit `sub`/`sbb` to borrow across the 64-bit value.
- Subtraction direction is `dp[disp] -= sp[0]` (dest minus source).
- No overflow trap.
