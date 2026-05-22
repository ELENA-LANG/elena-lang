# 0xDA -- ixor dp

- **Category:** TripleOp
- **Enum:** `ByteCode::IXorDPN`
- **Operand(s):** `ixor dp:disp, n` (n = 1, 2, 4, 8)
- **Reads:** `sp[0]` (n bytes), `dp[disp]` (n bytes)
- **Writes:** `dp[disp]` (n bytes)

## Semantics
`temp := n << sp[0]; dp[disp] ^= temp` -- width-selected bitwise XOR in place.

## JIT (amd64)
**Selection rule:** dispatched through `loadDPNOp` -> `retrieveICode(scope, arg2)`. Width `n`  in  {1,2,4,8} -> slots `%1DAh, %2DAh, %0DAh, %4DAh`. On x32 the n=8 form splits into two 32-bit XORs.

**Template:** load sp[0] into `rax`, XOR into `[rbp + disp]`.

```asm
; %0DAh -- n = 4
lea  rdi, [rbp + __arg32_1]
mov  rax, [r10]
xor  dword ptr [rdi], eax
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1DAh` | 1 | `xor byte ptr [rdi], al` |
| `%2DAh` | 2 | `xor word ptr [rdi], ax` |
| `%0DAh` | 4 | `xor dword ptr [rdi], eax` |
| `%4DAh` | 8 | `xor [rdi], rax` |

## JIT (x32)
**Template:** identical for 1/2/4; n=8 splits into two 32-bit XORs.

```asm
; %0DAh -- n = 4
lea  edi, [ebp + __arg32_1]
mov  eax, [esi]
xor  [edi], eax
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1DAh` | 1 | `xor byte ptr [edi], al` |
| `%2DAh` | 2 | `xor word ptr [edi], ax` |
| `%0DAh` | 4 | `xor [edi], eax` |
| `%4DAh` | 8 | `mov eax,[esi+4]; mov ecx,[esi]; xor [edi],ecx; xor [edi+4],eax` |

## JIT (aarch64)
**Template:** signed load + `eor` + width-appropriate store.

```asm
; %0DAh -- n = 4
add     x19, x29, __arg12_1
ldrsw   x17, [x0]
ldrsw   x18, [x19]
eor     x17, x17, x18
str     w17, [x19]
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1DAh` | 1 | `ldrsb/ldrsb; eor; strb` |
| `%2DAh` | 2 | `ldrsh/ldrsh; eor; strh` |
| `%0DAh` | 4 | `ldrsw/ldrsw; eor; str w` |
| `%4DAh` | 8 | `ldr/ldr; eor; str` |

## JIT (ppc64le)
**Template:** load/store wrapped around `xor`.

```asm
; %0DAh -- n = 4
addi    r19, r31, __arg16_1
lwz     r17, 0(r3)
lwz     r18, 0(r19)
xor     r17, r17, r18
stw     r17, 0(r19)
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1DAh` | 1 | `lbz/lbz; xor; stb` |
| `%2DAh` | 2 | `lhz/lhz; xor; sth` |
| `%0DAh` | 4 | `lwz/lwz; xor; stw` |
| `%4DAh` | 8 | `ld/ld; xor; std` |

## Notes
- The spec text for `inot dp:disp, n` (0xDB) also says "`^ temp`" -- it's a copy-paste artefact; that one is actually unary `~`. This opcode is the real XOR.
- Width `n`  in  {1,2,4,8} via `retrieveICode` on `arg2`.
- Same template family as `iand dp` (0xD8) / `ior dp` (0xD9).
- x32 splits the n=8 case into low/high 32-bit XORs.
