# 0x92 -- alloc i

- **Category:** DoubleOp
- **Enum:** `ByteCode::AllocI`
- **Operand(s):** `alloc i`
- **Reads:** --
- **Writes:** `sp`, freshly allocated slots
- **Side effects:** Stack must remain aligned to 16 on amd64/aarch64. `arg1` is the byte size; `__n_1` is the slot count.

## Semantics
`sp += i` -- grow the stack by `i` slots, fill them with zero. Small `i` is expanded inline; the general path zero-fills via a loop.

## Compiler behaviour
`compileAlloc` (`elenasrc3/engine/jitcompiler.cpp:2953-2961`) sets `scope->inlineMode = true` and (conditionally) zeroes `stackOffset`. The pair `alloc i` / `free i` (0x93) brackets a region where the JIT can emit stack-relative opcodes in a simpler form that does not track per-instruction offset changes. The matching `free` clears `inlineMode` and restores tracking.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %92h`

```asm
  sub  rsp, __arg32_1
  xor  rax, rax
  mov  ecx, __n_1
  mov  rdi, rsp
  rep  stos
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%92h`  | larger `i` | `rep stos` form above |
| `%192h` | `i == 0` | empty |
| `%292h` | `i == 1` | `push 0` |
| `%392h` | `i == 2` | two `push 0` |
| `%492h` | `i == 3` | three `push 0` |
| `%592h` | `i == 4` | four `push 0` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %92h`

```asm
  sub  esp, __arg32_1
  xor  eax, eax
  mov  ecx, __n_1
  mov  edi, esp
  rep  stos
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%92h`  | larger `i` | `rep stos` form above |
| `%192h` | `i == 0` | empty |
| `%292h` | `i == 1` | `push 0` |
| `%392h` | `i == 2` | two `push 0` |
| `%492h` | `i == 3` | three `push 0` |
| `%592h` | `i == 4` | four `push 0` |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %92h`

```asm
  sub     sp, sp,  __arg12_1   ; allocate stack
  mov     x11, __arg12_1
  mov     x12, 0
  mov     x13, sp

labLoop:
  cmp     x11, 0
  beq     labEnd
  sub     x11, x11, 8
  str     x12, [x13], #8
  b       labLoop

labEnd:
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%92h`  | larger `i` | zero-fill loop above |
| `%192h` | `i == 0` | empty |
| `%292h` | `i == 1` | single `str x12, [sp, #-8]!` |
| `%392h` | `i == 2` | `stp x12, x12, [sp, #-16]!` |
| `%492h` | `i == 3` | `stp` + extra `str` |
| `%592h` | `i == 4` | two `stp` pairs |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %92h`

```asm
  addi    r1, r1,  -__arg16_1   ; allocate stack
  li      r16, __arg16_1
  li      r17, 0
  mr      r18, r1

labLoop:
  cmpwi   r16,0
  beq     labEnd
  addi    r16, r16, -8
  std     r17, 0(r18)
  addi    r18, r18, 8
  b       labLoop

labEnd:
```

### Variants
| Prefix | Selected when | Body differs by |
|---|---|---|
| `%92h`  | larger `i` | zero-fill loop above |
| `%192h` | `i == 0` | empty |
| `%292h` | `i == 1` | `addi` + one `std r17, 0(r1)` |
| `%392h` | `i == 2` | `addi` + two `std` stores |
| `%492h` | `i == 3` | `addi` + three `std` stores |
| `%592h` | `i == 4` | `addi` + four `std` stores |

## Notes
- Pairs with `free i` (0x93); the JIT enters inline-mode between them and disables per-op stack tracking.
- Reserved stack is zero-filled (via `rep stos` / loop / per-slot `str`).
- Aligned to 16 bytes on amd64/aarch64 -- caller must size `i` to preserve alignment.
- Variants for `i in {0, 1, 2, 3, 4}` unroll the zero-fill; `i == 0` is a no-op.
