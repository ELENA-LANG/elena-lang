# 0xFC -- vcall mssg

- **Category:** TripleOp
- **Enum:** `ByteCode::VCallMR`
- **Operand(s):** `vcall mssg:m, r` (M+R)

## Semantics
Indirect call through the VMT message table. Loads `acc.VMT` and calls the slot at offset `m`. In alt mode (`%06FCh`), uses the secondary table (size from `elVMTSizeOffset`).

## JIT (amd64)
**Selection rule:** M+R command. `V`-prefixed = virtual / late-bound -- the call always goes through the live VMT slot. The `%06FCh` "alt" slot is selected when `scope->altMode` was set by a preceding 0x31 `altmode`; it skips past the primary VMT to the secondary table whose size lives in `elVMTSizeOffset`.

**Template:** `asm/amd64/core60.asm` `inline %0FCh`
```asm
; default
mov  ecx, __arg32_1
mov  rax, [rbx - elVMTOffset]
call [rax + rcx + 8]
```

### Variants
| Prefix | Body |
|---|---|
| `%0FCh` | default VMT call |
| `%06FCh` | alt mode (secondary VMT) |

```asm
; alt %06FCh
mov  rax, [rbx - elVMTOffset]
mov  ecx, __arg32_1
mov  rdi, [rax - elVMTSizeOffset]
shl  rdi, 4
lea  rax, [rax + rdi]
call [rax + rcx + 8]
```

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0FCh`
```asm
; default
mov  ecx, __arg32_1
mov  eax, [ebx - elVMTOffset]
call [eax + ecx + 4]
```

### Variants
| Prefix | Body |
|---|---|
| `%0FCh` | default |
| `%06FCh` | alt mode |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0FCh`
```asm
; default
mov     x18, __arg16_1
sub     x14, x10, elVMTOffset
ldr     x17, [x14]
add     x17, x17, x18
ldr     x17, [x17, #8]
blr     x17
```

### Variants
| Prefix | Body |
|---|---|
| `%0FCh` | default |
| `%06FCh` | alt mode |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0FCh`
```asm
; default
ld       r16, -elVMTOffset(r15)
addi     r16, r16, __arg16_1
ld       r17, 8(r16)
mtctr    r17
bctrl
```

### Variants
| Prefix | Body |
|---|---|
| `%0FCh` | default |
| `%6FCh` | alt mode |

## Notes
- V-prefix = virtual / late-bound -- every call goes through the live VMT message table.
- Non-tail counterpart of `vjump mssg` (0xEC); contrast with `call mssg` (0xFD) which patches a direct target.
- `altmode` (0x31) gates the secondary VMT -- must be emitted immediately before this opcode.
- Used for ordinary virtual method calls (most user-level method invocations land here).
