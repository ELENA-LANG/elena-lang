# 0xEC -- vjump mssg

- **Category:** TripleOp
- **Enum:** `ByteCode::VJumpMR`
- **Operand(s):** `vjump mssg:m, r` (M+R)

## Semantics
Indirect jump through the VMT message table. Loads `acc.VMT` and jumps to the slot at offset `m`. In alt mode (`%06ECh`), uses the secondary jump table located after the primary VMT (size from `elVMTSizeOffset`).

## JIT (amd64)
**Selection rule:** M+R command. The `V`-prefixed form means *virtual* dispatch -- execution always goes through the message-table slot (no link-time patching). The `%06ECh` "alt" slot is picked when `scope->altMode` was set by a preceding 0x31 `altmode`; it skips past the primary VMT to a secondary jump table whose size lives in `elVMTSizeOffset`.

**Template:** `asm/amd64/core60.asm` `inline %0ECh`
```asm
; default mode
mov  rax, [rbx - elVMTOffset]
jmp  [rax + __arg32_1]
```

### Variants
| Prefix | Body |
|---|---|
| `%0ECh` | default: jump via primary VMT slot |
| `%06ECh` | alt mode: jump via secondary table after primary |

```asm
; alt mode %06ECh
mov  rax, [rbx - elVMTOffset]
mov  rdi, [rax - elVMTSizeOffset]
shl  rdi, 4
lea  rax, [rax + rdi]
jmp  [rax + __arg32_1]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0ECh`
```asm
; default
mov  eax, [ebx - elVMTOffset]
mov  eax, [eax + __arg32_1]
jmp  eax
```

### Variants
| Prefix | Body |
|---|---|
| `%0ECh` | default |
| `%06ECh` | alt mode (skip past primary VMT) |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0ECh`
```asm
; default
sub     x14, x10, elVMTOffset
ldr     x17, [x14]
add     x17, x17, __arg12_1
ldr     x17, [x17]
br      x17
```

### Variants
| Prefix | Body |
|---|---|
| `%0ECh` | default |
| `%6ECh` | alt mode |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0ECh`
```asm
; default
ld       r16, -elVMTOffset(r15)
ld       r17, __arg16_1(r16)
mtctr    r17
bctr
```

### Variants
| Prefix | Body |
|---|---|
| `%0ECh` | default |
| `%6ECh` | alt mode |

## Notes
- V-prefix = virtual / late-bound -- every call goes through the live VMT message table.
- Tail-call form of `vcall mssg` (0xFC); contrast with `jump mssg` (0xED) which patches a direct address.
- `altmode` (0x31) gates the secondary table -- must be emitted immediately before this opcode.
- Used for tail-position virtual dispatch (no return path).
