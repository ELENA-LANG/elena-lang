# 0x96 -- write n

- **Category:** DoubleOp
- **Enum:** `ByteCode::WriteN`
- **Operand(s):** `write n`
- **Reads:** `acc`, `sp[0]`, `index`
- **Writes:** memory at `[sp[0] + index*n ..]`
- **Side effects:** Scratch registers clobbered; product computed on 32 bits, overflow not detected.

## Semantics
`sp[0][index*n] << acc, n bytes` -- copy `n` bytes from `acc` into the destination buffer `sp[0] + index*n`. Inverse direction of `read n` (0x95).

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %96h`

**Selection rule:** Variant picked by element width `n` -- power-of-2 widths in {1, 2, 4, 8} get dedicated scaled-store short forms on x32; arbitrary `n` falls back to `rep movsb`.

```asm
  mov  ecx, __n_1
  mov  eax, edx
  imul eax, ecx
  mov  rdi, r10
  add  rdi, rax
  mov  rsi, rbx
  rep  movsb
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %96h`

```asm
  mov  eax, edx
  mov  ecx, __n_1
  imul eax, ecx
  mov  edi, esi
  add  esi, eax
  mov  eax, edi
  mov  edi, esi
  mov  esi, ebx
  rep  movsb
  mov  esi, eax
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%96h`  | arbitrary `n` | `rep movsb` form above |
| `%196h` | `n == 0` | empty |
| `%296h` | `n == 1` | byte store via scaled `esi+edx` |
| `%396h` | `n == 2` | word store via scaled `esi+edx*2` |
| `%596h` | `n == 4` | dword store via scaled `esi+edx*4` |
| `%796h` | `n == 8` | qword store (two dword moves) via `esi+edx*8` |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %96h`

```asm
  mov     x11, __n16_1
  mov     x13, x10
  mul     x14, x9, x11
  add     x12, x0, x14

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 1
  ldrb    w14, [x13], #1
  strb    w14, [x12], #1
  b       labLoop

labEnd:
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%96h`   | small `n` fits MOV imm | mov + byte-copy loop above |
| `%0A96h` | wider `n` | MOVZ/MOVK 16-bit pair feeds the same loop |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %96h`

```asm
  li      r16, __n16_1      ; n
  mr      r18, r15
  mulld   r20, r16, r14
  add     r19, r3, r20

labLoop:
  cmpwi   r16,0
  beq     labEnd
  ld      r17, 0(r18)
  addi    r16, r16, -1
  stb     r17, 0(r19)
  addi    r18, r18, 1
  addi    r19, r19, 1
  b       labLoop

labEnd:
```

## Notes
- Inverse direction of `read n` (0x95); together they implement indexed buffer access.
- Destination offset is computed as `index * n` on 32 bits -- overflow is silent.
- Power-of-2 `n` variants (`{1, 2, 4, 8}`) on x32 use scaled addressing.
- No GC write barrier -- caller must ensure the destination is a primitive buffer.
