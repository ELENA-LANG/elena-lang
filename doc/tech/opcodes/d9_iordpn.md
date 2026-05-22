# 0xD9 -- ior dp

- **Category:** TripleOp
- **Enum:** `ByteCode::IOrDPN`
- **Operand(s):** `ior dp:disp, n` (n = 1, 2, 4, 8)
- **Reads:** `sp[0]` (n bytes), `dp[disp]` (n bytes)
- **Writes:** `dp[disp]` (n bytes)

## Semantics
`temp := n << sp[0]; dp[disp] |= temp` -- width-selected bitwise OR in place.

## JIT (amd64)
**Selection rule:** dispatched through `loadDPNOp` -> `retrieveICode(scope, arg2)`. Width `n`  in  {1,2,4,8} -> slots `%1D9h, %2D9h, %0D9h, %4D9h`. On x32 the n=8 form splits into two 32-bit ORs.

**Template:** load sp[0] into `rax`, OR into `[rbp + disp]`.

```asm
; %0D9h -- n = 4
lea  rdi, [rbp + __arg32_1]
mov  rax, [r10]
or   dword ptr [rdi], eax
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1D9h` | 1 | `or byte ptr [rdi], al` |
| `%2D9h` | 2 | `or word ptr [rdi], ax` |
| `%0D9h` | 4 | `or dword ptr [rdi], eax` |
| `%4D9h` | 8 | `or [rdi], rax` |

## JIT (x32)
**Template:** identical for 1/2/4; n=8 splits into low/high 32-bit ORs.

```asm
; %0D9h -- n = 4
lea  edi, [ebp + __arg32_1]
mov  eax, [esi]
or   [edi], eax
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1D9h` | 1 | `or byte ptr [edi], al` |
| `%2D9h` | 2 | `or word ptr [edi], ax` |
| `%0D9h` | 4 | `or [edi], eax` |
| `%4D9h` | 8 | `mov eax,[esi+4]; mov ecx,[esi]; or [edi],ecx; or [edi+4],eax` |

## JIT (aarch64)
**Template:** signed load + `orr` + width-appropriate store.

```asm
; %0D9h -- n = 4
add     x19, x29, __arg12_1
ldrsw   x17, [x0]
ldrsw   x18, [x19]
orr     x17, x17, x18
str     w17, [x19]
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1D9h` | 1 | `ldrsb/ldrsb; orr; strb` |
| `%2D9h` | 2 | `ldrsh/ldrsh; orr; strh` |
| `%0D9h` | 4 | `ldrsw/ldrsw; orr; str w` |
| `%4D9h` | 8 | `ldr/ldr; orr; str` |

## JIT (ppc64le)
**Template:** width-appropriate load/store around `or`.

```asm
; %0D9h -- n = 4
addi    r19, r31, __arg16_1
lwz     r17, 0(r3)
lwz     r18, 0(r19)
or      r17, r17, r18
stw     r17, 0(r19)
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1D9h` | 1 | `lbz/lbz; or; stb` |
| `%2D9h` | 2 | `lhz/lhz; or; sth` |
| `%0D9h` | 4 | `lwz/lwz; or; stw` |
| `%4D9h` | 8 | `ld/ld; or; std` |

## Notes
- Width `n`  in  {1,2,4,8} via `retrieveICode` on `arg2`.
- Same template family as `iand dp` (0xD8) / `ixor dp` (0xDA).
- Read-modify-write on `dp[disp]`.
- x32 splits the n=8 case into low/high 32-bit ORs.
