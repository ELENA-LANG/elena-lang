# 0x77 -- xsave n

- **Category:** DoubleOp
- **Enum:** `ByteCode::XSaveN`
- **Operand(s):** `n` (immediate 32-bit value)
- **Reads:** `acc` (pointer)
- **Writes:** `dword [acc]`

## Semantics
Stores the immediate constant `n` as a 32-bit value at the address held in `acc`: `dword [acc] := n`. Not listed in `bytecode60.txt`; semantics inferred from the JIT templates. A `n=0` variant compiles to an XOR-and-store on x86. Wider-immediate variants (`%977h`, `%A77h` etc.) handle values that don't fit a sign-extended 16-bit literal on RISC backends.

## JIT (amd64)
**Template:** `asm/amd64/core60.asm` `inline %77h`

```asm
  mov  eax, __n_1
  mov  dword ptr [rbx], eax
```

### Variants
| Prefix | Selected when | Body |
|---|---|---|
| `%077h` | base | `mov eax, n; mov dword [rbx], eax` |
| `%177h` | n = 0 | `xor eax, eax; mov dword [rbx], eax` |

## JIT (x32)
**Template:** `asm/x32/core60.asm` `inline %77h`

```asm
  mov  eax, __n_1
  mov  dword ptr [ebx], eax
```

### Variants
| Prefix | Selected when | Body |
|---|---|---|
| `%077h` | base | `mov eax, n; mov dword [ebx], eax` |
| `%177h` | n = 0 | `xor eax, eax; mov dword [ebx], eax` |

## JIT (aarch64)
**Template:** `asm/aarch64/core60.asm` `inline %77h`

```asm
  mov    x18, __n16_1
  str    w18, [x10]
```

### Variants
| Prefix | Selected when | Body |
|---|---|---|
| `%077h` | n fits in 16 bits | `mov x18, n; str w18, [x10]` |
| `%0977h` | wide immediate | `movz x18, lo; movk x18, hi, lsl #16; str w18, [x10]` |
| `%0A77h` | wide immediate (alt) | `movz x18, lo; movk x18, hi, lsl #16; str w18, [x10]` |

## JIT (ppc64le)
**Template:** `asm/ppc64le/core60.asm` `inline %77h`

```asm
  li      r18, __n16_1
  stw     r18, 0(r15)
```

### Variants
| Prefix | Selected when | Body |
|---|---|---|
| `%077h` | n fits in 16 bits | `li r18, n; stw r18, 0(r15)` |
| `%0977h` | wide immediate | `lis r18, hi; li r19, lo; andi. r19,r19,0FFFFh; add r18,r18,r19; stw r18, 0(r15)` |
| `%0A77h` | wide immediate (alt) | `li r18, lo; andi. r18,r18,0FFFFh; addis r18,r18,hi; stw r18, 0(r15)` |

## Notes
- Stores the 32-bit immediate `n` to `[acc]` -- used by constant-initializer / field-reset sequences.
- `n = 0` is special-cased on x86 to `xor eax, eax; mov [..], eax` (smaller encoding than `mov eax, 0`).
- aarch64/ppc64le route values that exceed the signed 16-bit literal range through `movz`/`movk` (aarch64) or `lis`+`li`+combine (ppc64le); the JIT picks the wider variant automatically.
- Writes only the low 32 bits at `[acc]` -- the high 32 bits of a qword slot are not touched by this opcode.
- `acc` and `index` are preserved; only memory at `[acc]` is updated.
