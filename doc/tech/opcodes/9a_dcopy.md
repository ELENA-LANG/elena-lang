# 0x9A -- dcopy

- **Category:** DoubleOp
- **Enum:** `ByteCode::DCopy`
- **Operand(s):** `dcopy n`
- **Reads:** `sp[0]`, `acc`, `index`
- **Writes:** memory at `[acc ..]`
- **Side effects:** Scratch registers clobbered; product computed on 32 bits -- overflow wraps silently.

## Semantics
`sp[0] >> acc, n*index bytes` -- like `copy n` (0x90) but the byte count is multiplied by the current `index`. Used to copy a variable-length block whose element count is in `index` and whose element size is `n`.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %9Ah`

```asm
  mov  rsi, r10
  mov  ecx, __n_1
  imul ecx, edx
  mov  rdi, rbx
  rep  movsb
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %9Ah`

```asm
  mov  ecx, __n_1
  imul ecx, edx
  mov  eax, esi
  mov  edi, ebx
  rep  movsb
  mov  esi, eax
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%9Ah`  | arbitrary `n` | `rep movsb` form above |
| `%29Ah` | `n == 1` | `ecx := index`, byte loop |
| `%39Ah` | `n == 2` | `ecx := index << 1`, byte loop |
| `%59Ah` | `n == 4` | `ecx := index`, `rep movsd` (dword stride) |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %9Ah`

```asm
  mov     x11, __n16_1
  mul     x11, x11, x9
  mov     x12, x0
  mov     x13, x10

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
**Template:** `asm/ppc64le/core60.asm` `inline %9Ah`

```asm
  li      r16, __n16_1
  mr      r19, r3
  mulld   r16, r16, r14
  mr      r18, r15

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
- Like `copy n` (0x90) but the byte count is `n * index` rather than `n`.
- Used to copy variable-length blocks where `index` holds the element count and `n` the element size.
- Byte count is computed on 32 bits -- overflow wraps silently.
- x32 variants for `n in {1, 2, 4}` switch between `rep movsb` and `rep movsd` strides.
