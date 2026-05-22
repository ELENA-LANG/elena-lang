# 0xAB -- lsave sp

- **Category:** DoubleOp
- **Enum:** `ByteCode::LSaveSI`
- **Operand(s):** `lsave sp:disp`
- **Reads:** `index` (full pointer-wide; `eax:edx` pair on x32)
- **Writes:** `sp[disp]` (full word)
- **Side effects:** None.

## Semantics
`sp[disp] := long:index` -- store the full-width `index` into stack slot `disp`. Long counterpart of `save sp:i` (0xA2); unlike that opcode there is no truncation through the 32-bit alias.

## JIT (amd64)

**Selection rule:** dispatched through `loadStackIndexOp` (`jitcompiler.cpp:898-907`) -- `retrieveCode(arg1)` picks the variant (`%0ABh` generic, `%1ABh` for `sp:0`, `%2ABh` for `sp:1`) and the operand slot is rewritten as `(scope->stackOffset + arg1) << indexPower`. The full-width form does not truncate `index` on 64-bit hosts.

**Template:** `asm/amd64/core60.asm` `inline %0ABh`

```asm
  mov qword ptr [rsp + __arg32_1], rdx
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0ABh` | generic `sp:disp` (disp > 1) | `mov [rsp+disp], rdx` |
| `%1ABh` | `sp:0`                       | `mov r10, rdx` |
| `%2ABh` | `sp:1`                       | `mov r11, rdx` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %0ABh`

```asm
  lea  edi, [esp + __arg32_1]
  mov [edi], eax
  mov [edi+4], edx
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0ABh` | generic `sp:disp` | spill `eax:edx` into `[esp+disp]` (low at `+0`, high at `+4`) |
| `%1ABh` | `sp:0`            | `mov eax, esi; xor edx, edx` |

`%1ABh` reloads the `eax:edx` long pair *from* the cached `sp[0]` slot (`esi`, low 32 bits only); the high half is cleared because the cache is intrinsically 32-bit on x32.

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %0ABh`

```asm
  add     x11, sp, __arg12_1
  str     x9, [x11]
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0ABh` | generic `sp:disp` (disp > 1) | `add x11, sp, disp; str x9, [x11]` |
| `%1ABh` | `sp:0`                       | `mov x0, x9` |
| `%2ABh` | `sp:1`                       | `mov x1, x9` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %0ABh`

```asm
  std     r14, __arg16_1(r1)
```

### Variants
| Prefix | Form | Body |
|---|---|---|
| `%0ABh` | generic `sp:disp` (disp > 1) | `std r14, disp(r1)` |
| `%1ABh` | `sp:0`                       | `mr r3, r14` |
| `%2ABh` | `sp:1`                       | `mr r4, r14` |

## Notes

- Full-width spill of `index` to `sp[disp]`. Unlike `save sp:i` (0xA2) which truncates to 32 bits, this preserves the entire pointer/long.
- On x32 the long lives in the `eax:edx` pair and is stored across two consecutive stack words; the `%1ABh` short form is asymmetric -- it *reloads* the pair from the cached `esi` (clearing `edx`) so subsequent long ops see a consistent register pair.
- The `%1ABh`/`%2ABh` short forms on 64-bit backends simply update the cached slot register.
- Stack slots are GC roots, so no write barrier is required.
