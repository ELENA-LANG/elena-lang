# 0x90 -- copy

- **Category:** DoubleOp
- **Enum:** `ByteCode::Copy`
- **Operand(s):** `copy n`
- **Reads:** `sp[0]`, `acc`
- **Writes:** memory at `[acc ..]`
- **Side effects:** Scratch registers clobbered.

## Semantics
`sp[0] >> acc, n bytes` -- block copy `n` bytes from the buffer pointed at by `sp[0]` into the buffer pointed at by `acc`. Specialised variants for `n = 1, 2, 4, 8` avoid the byte-loop overhead.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %90h`

```asm
  mov  rsi, r10
  mov  ecx, __n_1
  mov  rdi, rbx
  rep  movsb
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%90h`  | arbitrary `n` | `rep movsb` form above |
| `%290h` | `n == 1` | byte move via `al` |
| `%390h` | `n == 2` | word move via `ax` |
| `%590h` | `n == 4` | dword move via `eax` |
| `%790h` | `n == 8` | qword move via `rax` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %90h`

```asm
  mov  ecx, __n_1
  mov  edi, ebx
  rep  movsb
  sub  esi, __n_1          ; to set back ESI register
```

`esi` already holds `sp[0]`, so it doubles as the `movsb` source; the trailing `sub` restores `esi` because `rep movsb` advances it.

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%90h`  | arbitrary `n` | `rep movsb` form above |
| `%290h` | `n == 1` | byte move via `al` |
| `%390h` | `n == 2` | word move via `ax` |
| `%590h` | `n == 4` | dword move via `eax` |
| `%790h` | `n == 8` | `movq` via `xmm0` |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %90h`

```asm
  mov     x11, __n16_1
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

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%90h`   | small `n` fits MOV imm | mov + byte-copy loop above |
| `%0990h` | wider `n` | MOVZ/MOVK pair feeds the same loop |
| `%0A90h` | wider `n` (unsigned high half) | MOVZ/MOVK pair feeds the same loop |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %90h`

```asm
  li      r16, __n16_1
  mr      r19, r3
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
- Variants for `n in {1, 2, 4, 8}` avoid the byte-loop / `rep movsb` overhead with a single typed load/store.
- Companion `dcopy n` (0x9A) multiplies the byte count by `index` for variable-length blocks.
- x32 explicitly restores `esi` after `rep movsb` because `sp[0]` is cached there.
- No write barrier -- caller must ensure the destination is non-pointer or properly tracked.
