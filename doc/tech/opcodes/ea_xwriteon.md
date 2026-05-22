# 0xEA -- xwrite offs

- **Category:** TripleOp
- **Enum:** `ByteCode::XWriteON`
- **Operand(s):** `xwrite offs, n`

## Semantics
`acc[disp] >> sp[0]` for `n` bytes -- copy `n` bytes from `acc + disp` into the buffer at `sp[0]`.

## JIT (amd64)
**Selection rule:** `retrieveICode(n)` picks the variant by byte count: n  in  {1,2,4,8} -> `%1EAh..%4EAh`; otherwise the generic `%0EAh` `rep movsb` body.

**Template:** `asm/amd64/core60.asm` `inline %0EAh`
```asm
mov  rdi, r10
mov  ecx, __n_2
lea  rsi, [rbx + __arg32_1]
rep  movsb
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0EAh`
```asm
; generic
mov  eax, esi
mov  edi, esi
mov  ecx, __n_2
lea  esi, [ebx + __arg32_1]
rep  movsb
mov  esi, eax
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%0EAh` | arbitrary | `rep movsb` |
| `%1EAh` | 1 | `mov cl,[ebx+disp]; mov byte ptr [esi], cl` |
| `%2EAh` | 2 | `mov cx,[ebx+disp]; mov word ptr [esi], cx` |
| `%3EAh` | 4 | `mov ecx,[ebx+disp]; mov [esi], ecx` |
| `%4EAh` | 8 | dual 32-bit move |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0EAh`
```asm
mov     x11, __n16_2
mov     x14, __n16_1        ; disp
mov     x13, x0
add     x12, x10, x14

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 1
  ldrb    w14, [x12], #1
  strb    w14, [x13], #1
  b       labLoop

labEnd:
```

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0EAh`
```asm
li      r16, __n16_2
mr      r18, r3
addi    r19, r15, __n16_1

labLoop:
  cmpwi   r16,0
  beq     labEnd
  ld      r17, 0(r19)
  addi    r16, r16, -1
  stb     r17, 0(r18)
  addi    r18, r18, 1
  addi    r19, r19, 1
  b       labLoop

labEnd:
```

## Notes
- Direction: object field `acc[disp]` -> buffer at `sp[0]` (write from object out).
- Width `n` picks an inline sized variant; arbitrary widths use `rep movsb` (amd64/x32) or a byte loop (aarch64/ppc64le).
- On x32 the generic body must save/restore `esi` because it doubles as `sp[0]`.
- No GC barrier -- writes raw bytes, not pointer slots.
