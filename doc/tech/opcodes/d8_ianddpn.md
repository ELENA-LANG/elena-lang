# 0xD8 -- iand dp

- **Category:** TripleOp
- **Enum:** `ByteCode::IAndDPN`
- **Operand(s):** `iand dp:disp, n` (n = 1, 2, 4, 8)
- **Reads:** `sp[0]` (n bytes), `dp[disp]` (n bytes)
- **Writes:** `dp[disp]` (n bytes)

## Semantics
`temp := n << sp[0]; dp[disp] &= temp` -- width-selected bitwise AND in place.

## JIT (amd64)
**Selection rule:** dispatched through `loadDPNOp` -> `retrieveICode(scope, arg2)` (`jitcompiler.cpp:2399`). Width `n`  in  {1,2,4,8} -> slots `%1D8h, %2D8h, %0D8h, %4D8h` (n=4 is the default slot 0). On x32 the n=8 form splits into two 32-bit ANDs since there's no 64-bit register.

**Template:** load sp[0] into `rax`, AND into `[rbp + disp]` at the chosen width.

```asm
; %0D8h -- n = 4
lea  rdi, [rbp + __arg32_1]
mov  rax, [r10]
and  dword ptr [rdi], eax
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1D8h` | 1 | `and byte ptr [rdi], al` |
| `%2D8h` | 2 | `and word ptr [rdi], ax` |
| `%0D8h` | 4 | `and dword ptr [rdi], eax` |
| `%4D8h` | 8 | `and [rdi], rax` |

## JIT (x32)
**Template:** identical at 1/2/4 bytes; n=8 splits into two 32-bit ANDs.

```asm
; %0D8h -- n = 4
lea  edi, [ebp + __arg32_1]
mov  eax, [esi]
and  [edi], eax
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1D8h` | 1 | `and byte ptr [edi], al` |
| `%2D8h` | 2 | `and word ptr [edi], ax` |
| `%0D8h` | 4 | `and [edi], eax` |
| `%4D8h` | 8 | `mov eax,[esi+4]; mov ecx,[esi]; and [edi],ecx; and [edi+4],eax` |

## JIT (aarch64)
**Template:** sign-extending load at the chosen width, `and`, store back; n=8 uses full `ldr`.

```asm
; %0D8h -- n = 4
add     x19, x29, __arg12_1
ldrsw   x17, [x0]
ldrsw   x18, [x19]
and     x17, x17, x18
str     w17, [x19]
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1D8h` | 1 | `ldrsb/ldrsb; and; strb` |
| `%2D8h` | 2 | `ldrsh/ldrsh; and; strh` |
| `%0D8h` | 4 | `ldrsw/ldrsw; and; str w` |
| `%4D8h` | 8 | `ldr/ldr; and; str` |

## JIT (ppc64le)
**Template:** width-appropriate load/store wrapped around `and`.

```asm
; %0D8h -- n = 4
addi    r19, r31, __arg16_1
lwz     r17, 0(r3)
lwz     r18, 0(r19)
and     r17, r17, r18
stw     r17, 0(r19)
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1D8h` | 1 | `lbz/lbz; and; stb` |
| `%2D8h` | 2 | `lhz/lhz; and; sth` |
| `%0D8h` | 4 | `lwz/lwz; and; stw` |
| `%4D8h` | 8 | `ld/ld; and; std` |

## Notes
- Width `n`  in  {1,2,4,8} via `retrieveICode` slot picker on `arg2`.
- Sibling opcodes: `ior dp` (0xD9), `ixor dp` (0xDA), `inot dp` (0xDB) -- all four follow the same width-prefixed memory-target pattern except `inot` which is unary.
- Read-modify-write on `dp[disp]`; sp[0] supplies the right-hand operand.
- x32 splits the n=8 case into two 32-bit ANDs (low/high halves) since the host has no 64-bit GPRs.
