# 0xEB -- xcopy offs

- **Category:** TripleOp
- **Enum:** `ByteCode::XCopyON`
- **Operand(s):** `xcopy offs, n`

## Semantics
`sp[0] >> acc[disp]` for `n` bytes -- copy `n` bytes from `[sp[0]]` into `acc + disp` (the reverse of `xwrite`).

## JIT (amd64)
**Selection rule:** `retrieveICode(n)` picks by byte count: n  in  {1,2,4,8} -> sized inlines; otherwise `%0EBh` `rep movsb`.

**Template:** `asm/amd64/core60.asm` `inline %0EBh`
```asm
mov  rsi, r10
mov  ecx, __n_2
lea  rdi, [rbx + __arg32_1]
rep  movsb
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0EBh`
```asm
; generic
mov  ecx, __n_2
lea  edi, [ebx + __arg32_1]
rep  movsb
sub  esi, __n_2          ; to restore ESI register
```

### Variants
| Prefix | n | Body |
|---|---|---|
| `%0EBh` | arbitrary | `rep movsb` (adjusts esi back) |
| `%1EBh` | 1 | `mov al,[esi]; mov byte ptr [ebx+disp], al` |
| `%2EBh` | 2 | `mov ax,[esi]; mov word ptr [ebx+disp], ax` |
| `%3EBh` | 4 | `mov eax,[esi]; mov [ebx+disp], eax` |
| `%4EBh` | 8 | dual 32-bit move |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0EBh`
```asm
mov     x11, __n16_2
mov     x14, __n16_1        ; disp
mov     x12, x0
add     x13, x10, x14

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
**Template:** `asm/ppc64le/core60.asm` `inline %0EBh`
```asm
li      r16, __n16_2
mr      r19, r3
addi    r18, r15, __n16_1

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
- Direction: buffer at `sp[0]` -> object field `acc[disp]` (write into object).
- Inverse direction of `xwrite` (0xEA).
- x32 generic body adjusts `esi` back by `n` after `rep movsb` since `esi` is the live `sp[0]`.
- Width `n` picks an inline sized variant; arbitrary widths use `rep movsb` or a byte loop.
