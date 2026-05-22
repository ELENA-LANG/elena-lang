# 0xDB -- inot dp

- **Category:** TripleOp
- **Enum:** `ByteCode::INotDPN`
- **Operand(s):** `inot dp:disp, n` (n = 1, 2, 4, 8)
- **Reads:** `sp[0]` (n bytes)
- **Writes:** `dp[disp]` (n bytes)

## Semantics
`temp := n << sp[0]; dp[disp] := ~temp` -- width-selected bitwise NOT. The destination is overwritten with the complement of `sp[0]`; the previous value of `dp[disp]` is *not* read.

## JIT (amd64)
**Selection rule:** dispatched through `loadDPNOp` -> `retrieveICode(scope, arg2)` (`jitcompiler.cpp:2399`). Width `n`  in  {1,2,4,8} -> slots `%1DBh, %2DBh, %0DBh, %4DBh`. On x32 the n=8 form splits into two 32-bit halves with two separate `not` operations.

**Template:** load sp[0], `not`, store width-narrowed result.

```asm
; %0DBh -- n = 4
lea  rdi, [rbp + __arg32_1]
mov  rax, [r10]
not  rax
mov  dword ptr [rdi], eax
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1DBh` | 1 | `mov byte ptr [rdi], al` |
| `%2DBh` | 2 | `mov word ptr [rdi], ax` |
| `%0DBh` | 4 | `mov dword ptr [rdi], eax` |
| `%4DBh` | 8 | `mov [rdi], rax` |

## JIT (x32)
**Template:** load 32-bit, `not`, store; n=8 splits into two halves with two `not`s.

```asm
; %0DBh -- n = 4
lea  edi, [ebp + __arg32_1]
mov  eax, [esi]
not  eax
mov  [edi], eax
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1DBh` | 1 | `mov byte ptr [edi], al` |
| `%2DBh` | 2 | `mov word ptr [edi], ax` |
| `%0DBh` | 4 | `mov [edi], eax` |
| `%4DBh` | 8 | `mov eax,[esi+4]; mov ecx,[esi]; not eax; not ecx; mov [edi],ecx; mov [edi+4],eax` |

## JIT (aarch64)
**Template:** `mvn` (bitwise NOT), store at the chosen width.

```asm
; %0DBh -- n = 4
add     x19, x29, __arg12_1
ldrsw   x18, [x0]
mvn     x17, x18
str     w17, [x19]
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1DBh` | 1 | `ldrsw + mvn + strb` |
| `%2DBh` | 2 | `ldrsw + mvn + strh` |
| `%0DBh` | 4 | `ldrsw + mvn + str w` |
| `%4DBh` | 8 | `ldrsw + mvn + str` (note: the 8-byte form still loads 32-bit signed-extended) |

## JIT (ppc64le)
**Template:** `nand` of a register with itself realises NOT.

```asm
; %0DBh -- n = 4
addi    r19, r31, __arg16_1
lwz     r17, 0(r3)
nand    r18, r17, r17
stw     r18, 0(r19)
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%1DBh` | 1 | `lwz + nand + stb` |
| `%2DBh` | 2 | `lwz + nand + sth` |
| `%0DBh` | 4 | `lwz + nand + stw` |
| `%4DBh` | 8 | `lwz + nand + std` (also 32-bit-only source) |

## Notes
- Unlike its peers (`iand` / `ior` / `ixor`) this opcode is **unary** -- it does not depend on the current value of `dp[disp]`; the slot is overwritten.
- The asm spec wording uses `^` but the implementations clearly use `not` / `mvn` / `nand`.
- Width `n`  in  {1,2,4,8} via `retrieveICode` on `arg2`.
- ppc64le synthesises NOT via `nand reg, reg` (no dedicated `not` mnemonic).
- aarch64's n=8 form loads with `ldrsw` (32-bit sign-extended) -- verify if you need to NOT a true 64-bit source value.
